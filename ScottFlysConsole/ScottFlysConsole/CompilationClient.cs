using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Net.Http;
using System.Net.Http.Headers;
using System.IO;
using System.Text.RegularExpressions;

namespace ScottFlysConsole
{
    class CompilationClient
    {
        public static async Task<string> Compile(string url, string sourceFilename, string destinationFilename)
        {
            MemoryStream ms = new MemoryStream();
            using (FileStream fs = File.OpenRead(sourceFilename))
            {
                fs.CopyTo(ms);
            }
            using (var client = new HttpClient())
            {
                using (var content = new MultipartFormDataContent("Upload----" + DateTime.Now))
                {
                    content.Add(new StreamContent(new FileStream(sourceFilename, FileMode.Open)), "fileToCompile", Path.GetFileName(sourceFilename));
                    using (var message = await client.PostAsync(url, content))
                    {
                        var result = await message.Content.ReadAsStringAsync();
                        object o = message.Content.Headers.ContentDisposition;
                        if (o != null)
                        {
                            File.WriteAllText(destinationFilename, result);
                            return null;
                        } 
                        else
                        {
                            return result;
                        }

                    }
                }
            }
        }
    }
}
