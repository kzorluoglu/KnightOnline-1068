using System;
using System.IO;
using System.Text;
using System.Data;
using System.Collections.Generic;

namespace TblCli
{
    class Program
    {
        static void Main(string[] args)
        {
            // Register code page provider for Windows-1252
            Encoding.RegisterProvider(CodePagesEncodingProvider.Instance);
            try
            {
                Encoding.GetEncoding(1252);
            }
            catch (Exception)
            {
                Console.Error.WriteLine("ERROR: Windows-1252 encoding is not available even after registering provider. Please ensure System.Text.Encoding.CodePages is available.");
                Environment.Exit(2);
            }
            if (args.Length < 1)
            {
                Console.WriteLine("Usage: TblCli <tblfile> [csvout]");
                return;
            }
            string tblPath = args[0];
            string csvOut = args.Length > 1 ? args[1] : null;
            if (!File.Exists(tblPath))
            {
                Console.WriteLine($"File not found: {tblPath}");
                return;
            }
            byte[] decoded = LoadAndDecodeFile(tblPath);
            var table = ParseTbl(decoded);
            if (csvOut != null)
            {
                File.WriteAllText(csvOut, ToCsv(table));
                Console.WriteLine($"CSV written to {csvOut}");
            }
            else
            {
                Console.WriteLine(ToCsv(table));
            }
        }

        static byte[] LoadAndDecodeFile(string fileName)
        {
            byte[] fileData = File.ReadAllBytes(fileName);
            uint modWord = 0x0816;
            for (int i = 0; i < fileData.Length; i++)
            {
                byte encodedByte = fileData[i];
                uint tmpModWord = modWord;
                byte decodedByte = 0;
                tmpModWord &= 0xff00;
                tmpModWord = tmpModWord >> 8;
                decodedByte = (byte)(tmpModWord ^ encodedByte);
                tmpModWord = encodedByte;
                tmpModWord += modWord;
                tmpModWord = tmpModWord & 0xffff;
                tmpModWord = tmpModWord * 0x6081;
                tmpModWord = tmpModWord & 0xffff;
                tmpModWord += 0x1608;
                tmpModWord = tmpModWord & 0xffff;
                modWord = tmpModWord;
                fileData[i] = decodedByte;
            }
            return fileData;
        }

        static DataTable ParseTbl(byte[] data)
        {
            var table = new DataTable();
            int offset = 0;
            int colCount = BitConverter.ToInt32(data, offset); offset += 4;
            var colTypes = new int[colCount];
            for (int i = 0; i < colCount; i++)
            {
                colTypes[i] = BitConverter.ToInt32(data, offset); offset += 4;
            }
            for (int i = 0; i < colCount; i++)
            {
                table.Columns.Add($"Col{i}", TblTypeToType(colTypes[i]));
            }
            int rowCount = BitConverter.ToInt32(data, offset); offset += 4;
            for (int r = 0; r < rowCount; r++)
            {
                var row = table.NewRow();
                for (int c = 0; c < colCount; c++)
                {
                    row[c] = ReadValue(data, ref offset, colTypes[c]);
                }
                table.Rows.Add(row);
            }
            return table;
        }

        static Type TblTypeToType(int t)
        {
            switch (t)
            {
                case 8: return typeof(float);
                case 7: return typeof(string);
                case 6: return typeof(uint);
                case 5: return typeof(int);
                case 3: return typeof(short);
                case 2: return typeof(byte);
                case 1: return typeof(sbyte);
                default: return typeof(int);
            }
        }

        static object ReadValue(byte[] data, ref int offset, int t)
        {
            switch (t)
            {
                case 8:
                    float f = BitConverter.ToSingle(data, offset); offset += 4; return f;
                case 7:
                    int strlen = BitConverter.ToInt32(data, offset); offset += 4;
                    if (strlen == 0) return "";
                    string s = Encoding.GetEncoding(1252).GetString(data, offset, strlen); offset += strlen; return s;
                case 6:
                    uint u = BitConverter.ToUInt32(data, offset); offset += 4; return u;
                case 5:
                    int i = BitConverter.ToInt32(data, offset); offset += 4; return i;
                case 3:
                    short sh = BitConverter.ToInt16(data, offset); offset += 2; return sh;
                case 2:
                    byte b = data[offset]; offset += 1; return b;
                case 1:
                    sbyte sb = (sbyte)data[offset]; offset += 1; return sb;
                default:
                    int d = BitConverter.ToInt32(data, offset); offset += 4; return d;
            }
        }

        static string ToCsv(DataTable table)
        {
            var sb = new StringBuilder();
            for (int i = 0; i < table.Columns.Count; i++)
            {
                sb.Append(table.Columns[i].ColumnName);
                if (i < table.Columns.Count - 1) sb.Append(",");
            }
            sb.AppendLine();
            foreach (DataRow row in table.Rows)
            {
                for (int i = 0; i < table.Columns.Count; i++)
                {
                    string val = row[i]?.ToString()?.Replace("\"", "\"\"") ?? "";
                    if (val.Contains(",") || val.Contains("\""))
                        val = $"\"{val}\"";
                    sb.Append(val);
                    if (i < table.Columns.Count - 1) sb.Append(",");
                }
                sb.AppendLine();
            }
            return sb.ToString();
        }
    }
}
