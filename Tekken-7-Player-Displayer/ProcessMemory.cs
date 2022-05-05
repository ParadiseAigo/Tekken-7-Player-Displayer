using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Text;

namespace Tekken_7_Player_Displayer_csharp
{
    /// <summary>
    /// This class contains methods to open, read and write the memory of another process.
    /// Usage examples:
    ///  - Attaching: while (!Memory.Attach("Processname")) { Thread.Sleep(2000); }
    ///  - GetModuleAddress: int MyDll = Memory.GetModuleAddress("MyDll.dll");
    ///  - Writing: Memory.WriteMemory<type>(offset, value);
    ///  - Reading: var Value = Memory.ReadMemory<type>(offset);
    /// </summary>
    class ProcessMemory
    {
        [DllImport("kernel32.dll")]
        private static extern IntPtr OpenProcess(int dwDesiredAccess, bool bInheritHandle, int dwProcessId);

        [DllImport("kernel32.dll")]
        private static extern bool ReadProcessMemory(IntPtr hProcess, long lpBaseAddress, byte[] buffer, int size, out int lpNumberOfBytesRead);

        [DllImport("kernel32.dll")]
        private static extern bool WriteProcessMemory(IntPtr hProcess, long lpBaseAddress, byte[] buffer, int size, out int lpNumberOfBytesWritten);
        
        public static Process Process { get; private set; }
        public static IntPtr ProcessHandle { get; private set; }

        private static int bytesWritten;
        private static int bytesRead;

        internal struct Flags
        {
            public const int PROCESS_VM_OPERATION = 0x0008;
            public const int PROCESS_VM_READ = 0x0010;
            public const int PROCESS_VM_WRITE = 0x0020;
        }

        public static bool Attach(string ProcName)
        {
            Process[] processes = Process.GetProcessesByName(ProcName);
            if (processes.Length > 0)
            {
                Process = processes[0];
                ProcessHandle = OpenProcess(Flags.PROCESS_VM_OPERATION | Flags.PROCESS_VM_READ | Flags.PROCESS_VM_WRITE, false, Process.Id);
                return true;
            }

            return false;
        }

        public static void WriteMemory<T>(long Address, object Value)
        {
            var buffer = StructureToByteArray(Value);

            WriteProcessMemory(ProcessHandle, Address, buffer, buffer.Length, out bytesWritten);
        }

        public static void WriteMemory<T>(long Adress, char[] Value)
        {
            var buffer = Encoding.UTF8.GetBytes(Value);

            WriteProcessMemory(ProcessHandle, Adress, buffer, buffer.Length, out bytesWritten);
        }

        public static T ReadMemory<T>(long address) where T : struct
        {
            var ByteSize = Marshal.SizeOf(typeof(T));

            var buffer = new byte[ByteSize];

            ReadProcessMemory(ProcessHandle, address, buffer, buffer.Length, out bytesRead);

            return ByteArrayToStructure<T>(buffer);
        }

        public static byte[] ReadMemory(long offset, int size)
        {
            var buffer = new byte[size];
            ReadProcessMemory(ProcessHandle, offset, buffer, size, out bytesRead);

            return buffer;
        }

        public static float[] ReadFloat<T>(int Adress, int MatrixSize) where T : struct
        {
            var ByteSize = Marshal.SizeOf(typeof(T));
            var buffer = new byte[ByteSize * MatrixSize];
            ReadProcessMemory(ProcessHandle, Adress, buffer, buffer.Length, out bytesRead);

            return ConvertToFloatArray(buffer);
        }

        public static string ReadString(long addr)
        {
            int maxLength = 50;
            byte[] b = ReadMemory(addr, maxLength);
            string s = Encoding.ASCII.GetString(b);
            int i = s.IndexOf('\0');
            if (i >= 0) s = s.Remove(i);
            return s;
        }

        /// <summary>
        /// Reads a 64-bit int from the specified address.
        /// </summary>
        /// <param name="hProcess">The handle of the process to read from.</param>
        /// <param name="memaddress">The memory address we start reading from.</param>
        /// <returns></returns>
        internal static long ReadInt64(long memaddress)
        {
            // Read the bytes using the other function
            byte[] bytes = ReadMemory(memaddress, 8);
            return BitConverter.ToInt64(bytes, 0);
        }

        public static long GetModuleAddress(string Name)
        {
            try
            {
                foreach (ProcessModule ProcMod in Process.Modules)
                    if (Name == ProcMod.ModuleName)
                        return (long)ProcMod.BaseAddress;
            }
            catch
            {
                return 0;
            }
            return 0;
        }

        /// <summary>
        /// Returns a direct address to the value. Do not store this address to reliably
        /// get data since it can change at any time.
        /// </summary>
        /// <returns>The current address to the data, otherwise 0</returns>
        public static long GetDynamicAddress(long address, int[] offsets)
        {
            try
            {
                // Iteratively update the current address by moving up the pointers chain
                foreach (int offset in offsets)
                    address = ReadInt64(address) + offset;

                return address;
            }
            catch (UnauthorizedAccessException)
            {
                return 0; // Pointer is probably changing addresses
            }
        }

        public static bool IsMemoryReadable(long address)
        {
            int size = 1;
            var buffer = new byte[size];
            bool success = ReadProcessMemory(ProcessHandle, address, buffer, size, out bytesRead);
            return success;
        }

        public static float[] ConvertToFloatArray(byte[] bytes)
        {
            if (bytes.Length % 4 != 0)
                throw new ArgumentException();

            var floats = new float[bytes.Length / 4];

            for (var i = 0; i < floats.Length; i++)
                floats[i] = BitConverter.ToSingle(bytes, i * 4);

            return floats;
        }

        private static T ByteArrayToStructure<T>(byte[] bytes) where T : struct
        {
            var handle = GCHandle.Alloc(bytes, GCHandleType.Pinned);
            try
            {
                return (T)Marshal.PtrToStructure(handle.AddrOfPinnedObject(), typeof(T));
            }
            finally
            {
                handle.Free();
            }
        }

        private static byte[] StructureToByteArray(object obj)
        {
            var length = Marshal.SizeOf(obj);

            var array = new byte[length];

            var pointer = Marshal.AllocHGlobal(length);

            Marshal.StructureToPtr(obj, pointer, true);
            Marshal.Copy(pointer, array, 0, length);
            Marshal.FreeHGlobal(pointer);

            return array;
        }
    }
}
