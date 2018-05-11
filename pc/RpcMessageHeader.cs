using System.Runtime.InteropServices;

namespace Oni
{
    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi, Pack = 1)]
    public struct RpcMessageHeader
    {
        /// <summary>
        /// Magic byte, default: 0xCC
        /// </summary>
        public byte Magic;

        /// <summary>
        /// Bool value representing if this current message is a Request or reply
        /// </summary>
        public byte Request;

        /// <summary>
        /// The length of the payload, which comes after this header
        /// </summary>
        public ushort PayloadLength;

        /// <summary>
        /// RPC Category
        /// </summary>
        public int Category;

        /// <summary>
        /// Command CommandType
        /// </summary>
        public uint CommandType;

        /// <summary>
        /// Error, if this is non-zero then do not read any payload
        /// </summary>
        public int Error;
    }
}
