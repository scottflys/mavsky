using System;
using System.Data;
using System.Linq;
using System.Windows.Forms;
using System.IO.Ports;
using System.IO;
using System.Management;
using System.Threading.Tasks;

namespace ScottFlysConsole
{
    public partial class Form1 : Form
    {
        const string defaultSourceExtension = "txt";
        const string defaultCompiledExtension = "dat";
        SerialPort serialPort;
        bool connected = false;

        public Form1()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            using (var searcher = new ManagementObjectSearcher("SELECT * FROM WIN32_SerialPort"))
            {
                string lastConnectedPort = Properties.Settings.Default.LastConnectedPort;

                string[] portNames = SerialPort.GetPortNames();
                var ports = searcher.Get().Cast<ManagementBaseObject>().ToList();
                var portDictionary = (from n in portNames
                    join p in ports on n equals p["DeviceID"].ToString()
                    select p).ToDictionary(k => k["DeviceID"], v => v["Caption"]);

                foreach (string portName in portDictionary.Keys)
                {
                    string portDescription = portDictionary[portName].ToString();
                    int i = cmbSerialPorts.Items.Add(new ComboBoxItem(portName, portDescription));
                    if(portName == lastConnectedPort)
                    {
                        cmbSerialPorts.SelectedIndex = i;
                    }
                }
            }
            uploadToolStripMenuItem.Enabled = false;
            compileAndUploadToolStripMenuItem.Enabled = false;
        }

        private void connect_Click(object sender, EventArgs e)
        {
            if (connected)
            {
                connected = false;
                serialPort.Close();
                btnConnect.Text = "Connect";
                uploadToolStripMenuItem.Enabled = false;
                compileAndUploadToolStripMenuItem.Enabled = false;
            }
            else
            {
                string portName = ((ComboBoxItem)cmbSerialPorts.SelectedItem).HiddenValue;
                serialPort = new SerialPort();
                serialPort.PortName = portName;
                serialPort.BaudRate = 9600;
                serialPort.Parity = Parity.None;
                serialPort.DataBits = 8;
                serialPort.StopBits = StopBits.Two;
                serialPort.Handshake = Handshake.None;
                serialPort.ReadTimeout = 500;
                serialPort.WriteTimeout = 500;

                serialPort.Open();

                Properties.Settings.Default.LastConnectedPort = portName;
                Properties.Settings.Default.Save();

                connected = true;
                btnConnect.Text = "Disconnect";
                uploadToolStripMenuItem.Enabled = true;
                compileAndUploadToolStripMenuItem.Enabled = true;

                txtConsole.Focus();

                serialPort.Write("\r");

                while (connected)
                {
                    try
                    {
                        if (serialPort.BytesToRead > 0)
                        {
                            byte[] buffer = new byte[serialPort.ReadBufferSize];
                            int bytesRead = serialPort.Read(buffer, 0, buffer.Length);
                            if (bytesRead > 0)
                            {
                                string s = System.Text.Encoding.UTF8.GetString(buffer, 0, bytesRead);
                                txtConsole.Text += s;
                                txtConsole.Select(txtConsole.Text.Length, 0);
                                txtConsole.ScrollToCaret();
                            }
                        }
                        Application.DoEvents();
                    }
                    catch (TimeoutException) { }
                }
            }
        }

        private void txtConsole_KeyPress(object sender, KeyPressEventArgs e)
        {
            char c = e.KeyChar;
            serialPort.Write(c.ToString());
            e.Handled = true;
        }

        private void Form1_Resize(object sender, EventArgs e)
        {
            txtConsole.Width = this.Width - 40;
            txtConsole.Height = this.Height - 90;
            cmbSerialPorts.Left = this.Width - 355;
            btnConnect.Left = this.Width - 100;
        }

        private async Task<string> doCompile()
        {
            OpenFileDialog openFileDialog = new OpenFileDialog();
            openFileDialog.Filter = string.Format("Source Files (.{0})|*.{0}|All Files (*.*)|*.*", defaultSourceExtension);
            openFileDialog.FilterIndex = 1;
            DialogResult dialogResult = openFileDialog.ShowDialog();
            if (dialogResult == DialogResult.OK)
            {
                string sourceFilename = openFileDialog.FileName;
                string destinationFilename = Path.Combine(Path.GetDirectoryName(sourceFilename), Path.GetFileNameWithoutExtension(sourceFilename)) + "." + defaultCompiledExtension;
                try
                {
                    string compileResult = await CompilationClient.Compile("http://watsys.com/compile_raw.php", sourceFilename, destinationFilename);
                    if (compileResult != null)
                    {
                        MessageBox.Show(this, compileResult, "Compilation Error");
                        return null;
                    }
                    else
                    {
                        return destinationFilename;
                    }
                }
                catch (Exception ex)
                {
                    Console.WriteLine(ex.Message);
                }
            }
            return null;
        }

        private async void compileFileToolStripMenuItem_Click(object sender, EventArgs e)
        {
            string compiledFilename = await doCompile();
            if (compiledFilename != null)
            {
                MessageBox.Show(this, string.Format("Successfully compiled to {0}", compiledFilename), "Status");
            }
        }

        private bool doUpload(string filename)
        {
            try
            {
                string[] lines = File.ReadAllLines(filename);
                foreach (string line in lines)
                {
                    Console.WriteLine(line);
                    serialPort.Write(line + "\r");
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.Message);
                return false;
            }
            return true;
        }

        private void uploadToolStripMenuItem_Click(object sender, EventArgs e)
        {
            OpenFileDialog openFileDialog = new OpenFileDialog();
            openFileDialog.Filter = string.Format("Compiled Files (.{0})|*.{0}|All Files (*.*)|*.*", defaultCompiledExtension);
            openFileDialog.FilterIndex = 1;
            DialogResult dialogResult = openFileDialog.ShowDialog();
            if (dialogResult == DialogResult.OK)
            {
                string filename = openFileDialog.FileName;
                doUpload(filename);
            }
        }

        private async void compileAndUploadToolStripMenuItem_Click(object sender, EventArgs e)
        {
            string compiledFilename = await doCompile();
            if (compiledFilename != null)
            {
                doUpload(compiledFilename);
            }
        }

        private void exitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Application.Exit();
        }

    }
}
