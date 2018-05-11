/*
 *  Oni.OniConnection
 *  Created by: kiwidog (keybase.io/kiwidog)
 *  
 *  This is the class responsible for the basic communications
 *  I really need to refactor this, but at this point I don't care.
 *  
 */
using System;
using System.IO;
using System.Net.Sockets;
using System.Runtime.InteropServices;
using System.Text;

namespace Oni
{
    /// <summary>
    /// Categories, these MUST MATCH THE C IMPLEMENTATION
    /// </summary>
    public enum RPC_CATEGORY
    {
        /// <summary>
        /// Nothing
        /// </summary>
        RPCCAT_NONE = 0,

        /// <summary>
        /// System category
        /// </summary>
        RPCCAT_SYSTEM,

        /// <summary>
        /// Logger category
        /// </summary>
        RPCCAT_LOG,

        /// <summary>
        /// Debugger category
        /// </summary>
        RPCCAT_DBG,

        /// <summary>
        /// File category
        /// </summary>
        RPCCAT_FILE,

        /// <summary>
        /// Command category
        /// </summary>
        RPCCAT_CMD,

        /// <summary>
        /// Category count
        /// </summary>
        RPCCAT_COUNT
    };

    /// <summary>
    /// The bread and butter
    /// </summary>
    public class OniConnection
    {
        // Header magic, THIS MUST MATCH C IMPLEMENTATION
        public const byte c_Magic = 0xCC;

        // Buffer size, THIS MUST MATCH C IMPLEMENTATION
        public const int c_BufferSize = 0x4000;

        // IP address
        protected readonly string m_Address;

        // Server port
        protected readonly ushort m_Port;

        // Client
        internal TcpClient m_Client;

        /// <summary>
        /// Returns if the client is connected
        /// </summary>
        public bool Connected => m_Client?.Connected ?? false;

        /// <summary>
        /// IP Address
        /// </summary>
        public string Address => m_Address;

        /// <summary>
        /// Creates a new connection instance
        /// </summary>
        /// <param name="p_IpAddress">Ip address of the server</param>
        /// <param name="p_Port">Port to connect to</param>
        public OniConnection(string p_IpAddress, ushort p_Port)
        {
            m_Address = p_IpAddress;
            m_Port = p_Port;
        }

        /// <summary>
        /// Connect to a remote client
        /// </summary>
        /// <returns>True on success, false otherwise</returns>
        public bool Connect()
        {
            try
            {
                m_Client = new TcpClient(m_Address, m_Port)
                {
                    // Large file transfers will stall if this is set to low
                    SendTimeout = 1000 * 10,
                    ReceiveTimeout = 1000 * 10,

                    // These must match the c implementation
                    SendBufferSize = c_BufferSize,
                    ReceiveBufferSize = c_BufferSize
                };
            }
            catch (Exception p_Exception)
            {
                Console.WriteLine(p_Exception);
                return false;
            }

            return m_Client.Connected;
        }

        /// <summary>
        /// Disconnects the client
        /// </summary>
        public void Disconnect()
        {
            try
            {
                m_Client.Close();
            }
            catch (Exception p_Exception)
            {
                Console.WriteLine(p_Exception);
            }
        }

        /// <summary>
        /// Serializes an object into a byte array
        /// </summary>
        /// <param name="p_Object">Structure to serialize</param>
        /// <returns>Byte array of object</returns>
        public byte[] SerializeObject(object p_Object)
        {
            var s_Size = Marshal.SizeOf(p_Object);

            var s_Data = new byte[s_Size];

            var s_Ptr = Marshal.AllocHGlobal(s_Size);

            Marshal.StructureToPtr(p_Object, s_Ptr, true);

            Marshal.Copy(s_Ptr, s_Data, 0, s_Size);

            Marshal.FreeHGlobal(s_Ptr);

            return s_Data;
        }

        /// <summary>
        /// Deserializes a object to the specified structure
        /// </summary>
        /// <typeparam name="T">Structure to deserialize</typeparam>
        /// <param name="p_Data">Incoming data to deserialize from</param>
        /// <returns>Object or null</returns>
        public T DeserializeObject<T>(byte[] p_Data)
        {
            var s_Size = Marshal.SizeOf<T>();
            if (p_Data.Length < s_Size)
                throw new InvalidDataException($"Data size is too small to deserialize {typeof(T).FullName}");

            var s_Ptr = Marshal.AllocHGlobal(s_Size);
            Marshal.Copy(p_Data, 0, s_Ptr, s_Size);
            var s_Object = (T)Marshal.PtrToStructure(s_Ptr, typeof(T));
            Marshal.FreeHGlobal(s_Ptr);

            return s_Object;
        }

        /// <summary>
        /// Receive's an object from over the network, will not accept data afterwards
        /// </summary>
        /// <typeparam name="T">Structure type to download</typeparam>
        /// <returns>Object created</returns>
        public T ReceiveObject<T>()
        {
            var s_Size = Marshal.SizeOf<T>();

            byte[] s_Data;
            using (var s_Reader = new BinaryReader(m_Client.GetStream(), Encoding.ASCII, true))
                s_Data = s_Reader.ReadBytes(s_Size);

            if (s_Data.Length < s_Size)
                throw new InvalidDataException("incoming data length < required size");

            var s_Object = DeserializeObject<T>(s_Data);

            return s_Object;
        }

        /// <summary>
        /// Sends a structure to the server
        /// </summary>
        /// <typeparam name="T">Type of structure</typeparam>
        /// <param name="p_Object">Structure to send</param>
        public void SendMessage<T>(T p_Object)
        {
            var s_Data = SerializeObject(p_Object);

            using (var s_Writer = new BinaryWriter(m_Client.GetStream(), Encoding.ASCII, true))
                s_Writer.Write(s_Data);
        }

        /// <summary>
        /// Sends a message with the supplied header, and payload
        /// </summary>
        /// <param name="p_Object">Header</param>
        /// <param name="p_Payload">Payload data</param>
        public void SendMessage(RpcMessageHeader p_Object, byte[] p_Payload)
        {
            var s_HeaderData = SerializeObject(p_Object);

            using (var s_Writer = new BinaryWriter(m_Client.GetStream(), Encoding.ASCII, true))
            {
                s_Writer.Write(s_HeaderData);
                s_Writer.Write(p_Payload);
            }
        }

        /// <summary>
        /// Sends a message with a structure payload
        /// </summary>
        /// <typeparam name="T">Type of payload</typeparam>
        /// <param name="p_Header">Header</param>
        /// <param name="p_Payload">Structure to send</param>
        public void SendMessage<T>(RpcMessageHeader p_Header, T p_Payload)
        {
            var s_HeaderData = SerializeObject(p_Header);
            var s_PayloadData = SerializeObject(p_Payload);

            using (var s_Writer = new BinaryWriter(m_Client.GetStream(), Encoding.ASCII, true))
            {
                s_Writer.Write(s_HeaderData);
                s_Writer.Write(s_PayloadData);
            }
        }
    }
}
