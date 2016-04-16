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
        public static string CompileSync(string url, string sourceFilename, string destinationFilename)
        {
            MemoryStream ms = new MemoryStream();
            using (FileStream fs = File.OpenRead(sourceFilename))
            {
                fs.CopyTo(ms);
            }
            using (var client = new HttpClient())
            {
                client.Timeout = new System.TimeSpan(0, 0, 10);
                using (var content = new MultipartFormDataContent("Upload----" + DateTime.Now))
                {
                    content.Add(new StreamContent(new FileStream(sourceFilename, FileMode.Open)), "fileToCompile", Path.GetFileName(sourceFilename));
                    try
                    {
                        using (var response = client.PostAsync(url, content))
                        {
                            var responseContent = response.Result.Content;
                            if (response.Result.IsSuccessStatusCode)
                            {
                                string responseString = responseContent.ReadAsStringAsync().Result;
                                var headers = responseContent.Headers;
                                if (headers.ContentDisposition != null)
                                {
                                    File.WriteAllText(destinationFilename, responseString);
                                    return null;
                                }
                                else
                                {
                                    return responseString;
                                }
                            }
                            else
                            {
                                throw new Exception(response.Result.ReasonPhrase);
                            }
                        }
                    }
                    catch (Exception ex)
                    {
                        throw new Exception(string.Format("There was a problem accessing {0}.  Please make sure you are connected to the internet.  [{1}]", url, ex.Message));
                    }
                }
            }
        }
    }
}
