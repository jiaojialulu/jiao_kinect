using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace UseCPPDLL
{
    
    class Program
    {
        [DllImport("DepthFilterDll.dll", EntryPoint = "DepthFilter", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        extern static IntPtr DepthFilter(string ImageIn, string ImageOut, int width, int height, int value);
        static void Main(string[] args)
        {
            IntPtr y;//初始化 略
            StringBuilder bsIn = new StringBuilder(320 * 240);
            StringBuilder bsOut = new StringBuilder(320 * 240);
            byte[] bsTemp = Enumerable.Repeat((byte)0x08, 320*240).ToArray();
            byte[] bsFilter = new byte[bsTemp.Length];
            string strIn = System.Text.Encoding.Default.GetString(bsTemp);
            string strOut = System.Text.Encoding.Default.GetString(bsFilter);
            y = DepthFilter(strIn, strOut, 320, 240, 10);
            Marshal.Copy(y, bsFilter, 0, bsTemp.Length);

            strOut = System.Text.Encoding.Default.GetString(bsFilter);
            Console.WriteLine(strOut);
        }
    }
}
