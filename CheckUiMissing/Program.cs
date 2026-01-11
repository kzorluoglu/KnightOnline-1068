using System;
using System.IO;
using System.Linq;

class Program
{
    static void Main()
    {
        string csvPath = "UIs.csv";
        string uiFolder = "1097 Client/UI_en/";
        if (!File.Exists(csvPath))
        {
            Console.WriteLine($"CSV not found: {csvPath}");
            return;
        }
        if (!Directory.Exists(uiFolder))
        {
            Console.WriteLine($"UI folder not found: {uiFolder}");
            return;
        }
        var lines = File.ReadAllLines(csvPath);
        if (lines.Length < 2)
        {
            Console.WriteLine("CSV missing data rows");
            return;
        }
        var missing = 0;
        var row = lines[1].Split(',');
        foreach (var entry in row)
        {
            var trimmed = entry.Trim().Replace("\\", "/");
            if (string.IsNullOrWhiteSpace(trimmed) || !trimmed.EndsWith(".uif", StringComparison.OrdinalIgnoreCase))
                continue;
            var fileName = Path.GetFileName(trimmed);
            var fullPath = Path.Combine(uiFolder, fileName);
            if (!File.Exists(fullPath))
            {
                Console.WriteLine($"Missing: {fullPath}");
                missing++;
            }
        }
        if (missing == 0)
            Console.WriteLine("All UI .uif files referenced in UIs.tbl exist in UI_en folder.");
        else
            Console.WriteLine($"Total missing: {missing}");
    }
}
