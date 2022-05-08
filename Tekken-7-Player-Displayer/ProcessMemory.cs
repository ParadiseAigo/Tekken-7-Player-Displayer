using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Text;

namespace Tekken_7_Player_Displayer
{
    /// <summary>
    /// This class contains methods to open, read and write the memory of another process.
    /// </summary>
    class ProcessMemory
    {
        [DllImport("kernel32.dll")]
        private static extern IntPtr OpenProcess(int dwDesiredAccess, bool bInheritHandle, int dwProcessId);

        [DllImport("kernel32.dll")]
        private static extern bool ReadProcessMemory(IntPtr hProcess, long lpBaseAddress, byte[] buffer, int size, out int lpNumberOfBytesRead);

        [DllImport("kernel32.dll")]
        private static extern bool WriteProcessMemory(IntPtr hProcess, long lpBaseAddress, byte[] buffer, int size, out int lpNumberOfBytesWritten);

        private struct Flags
        {
            public const int PROCESS_VM_OPERATION = 0x0008;
            public const int PROCESS_VM_READ = 0x0010;
            public const int PROCESS_VM_WRITE = 0x0020;
        }

        private static int bytesWritten;
        private static int bytesRead;

        public static Process Process { get; private set; }
        public static IntPtr ProcessHandle { get; private set; }

        public static bool Attach(string processName)
        {
            Process[] processes = Process.GetProcessesByName(processName);
            if (processes.Length > 0)
            {
                Process = processes[0];
                ProcessHandle = OpenProcess(Flags.PROCESS_VM_OPERATION | Flags.PROCESS_VM_READ | Flags.PROCESS_VM_WRITE, false, Process.Id);
                return true;
            }
            return false;
        }

        public static T ReadMemory<T>(long address) where T : struct
        {
            var byteSize = Marshal.SizeOf(typeof(T));
            var buffer = new byte[byteSize];
            ReadProcessMemory(ProcessHandle, address, buffer, buffer.Length, out bytesRead);
            return ByteArrayToStructure<T>(buffer);
        }

        public static string ReadString(long address)
        {
            int maxLength = 50;
            byte[] buffer = ReadMemory(address, maxLength);
            string text = Encoding.ASCII.GetString(buffer);
            int indexStringEnd = text.IndexOf('\0');
            if (indexStringEnd >= 0) text = text.Remove(indexStringEnd);
            return text;
        }

        public static float[] ReadFloat<T>(int address, int matrixSize) where T : struct
        {
            var byteSize = Marshal.SizeOf(typeof(T));
            var buffer = new byte[byteSize * matrixSize];
            ReadProcessMemory(ProcessHandle, address, buffer, buffer.Length, out bytesRead);
            return ConvertToFloatArray(buffer);
        }

        private static byte[] ReadMemory(long offset, int size)
        {
            var buffer = new byte[size];
            ReadProcessMemory(ProcessHandle, offset, buffer, size, out bytesRead);
            return buffer;
        }

        public static void WriteMemory<T>(long address, object value)
        {
            var buffer = StructureToByteArray(value);
            WriteProcessMemory(ProcessHandle, address, buffer, buffer.Length, out bytesWritten);
        }

        public static void WriteMemory<T>(long address, char[] value)
        {
            var buffer = Encoding.UTF8.GetBytes(value);
            WriteProcessMemory(ProcessHandle, address, buffer, buffer.Length + 1, out bytesWritten);
        }

        public static long GetModuleAddress(string moduleName)
        {
            try
            {
                foreach (ProcessModule processModule in Process.Modules)
                    if (moduleName == processModule.ModuleName)
                        return (long)processModule.BaseAddress;
            }
            catch { }
            return 0;
        }

        public static long GetDynamicAddress(long address, int[] offsets)
        {
            try
            {
                foreach (int offset in offsets) address = Read64BitInt(address) + offset;
                return address;
            }
            catch { }
            return 0;
        }

        private static long Read64BitInt(long address)
        {
            byte[] bytes = ReadMemory(address, 8);
            return BitConverter.ToInt64(bytes, 0);
        }

        public static bool IsMemoryReadable(long address)
        {
            int size = 1;
            var buffer = new byte[size];
            bool success = ReadProcessMemory(ProcessHandle, address, buffer, size, out bytesRead);
            return success;
        }

        private static float[] ConvertToFloatArray(byte[] bytes)
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
