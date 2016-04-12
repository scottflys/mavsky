//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  A copy of the GNU General Public License is available at <http://www.gnu.org/licenses/>.
//  

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
