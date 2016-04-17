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
using System.Data;
using System.Linq;
using System.Windows.Forms;
using System.IO.Ports;
using System.IO;
using System.Management;
using System.Threading;
using System.Threading.Tasks;

namespace ScottFlysConsole
{
    public partial class MainForm : Form
    {
        const string defaultSourceExtension = "txt";
        const string defaultCompiledExtension = "dat";
        //const int consoleHighWaterMark = 50000;
        //const int consoleLowWaterMark  = 40000;

        SerialPort serialPort;
        bool connected = false;

        public MainForm()
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

            moveSerialPortComboAndConnectButton();
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
                if(cmbSerialPorts.SelectedIndex < 0)
                {
                    MessageBox.Show(this, "Please select a COM port first", "Error");
                    return;
                }
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

                try
                {
                    serialPort.Open();
                    serialPort.Write("\rhelp\r");
                }
                catch(Exception ex)
                {
                    MessageBox.Show("Error.  Please check the device and retry");
                    return;
                }
                Properties.Settings.Default.LastConnectedPort = portName;
                Properties.Settings.Default.Save();

                connected = true;
                btnConnect.Text = "Disconnect";
                uploadToolStripMenuItem.Enabled = true;
                compileAndUploadToolStripMenuItem.Enabled = true;

                txtConsole.Focus();

                while (connected)
                {
                    try
                    {
                        pollSerial();
                        Application.DoEvents();
                    }
                    catch (Exception) { }
                }
            }
        }

        private void txtConsole_KeyPress(object sender, KeyPressEventArgs e)
        {
            char c = e.KeyChar;
            serialPort.Write(c.ToString());
            e.Handled = true;
        }

        private void moveSerialPortComboAndConnectButton()
        {
            btnConnect.Left = this.Width - btnConnect.Width - 30;
            cmbSerialPorts.Left = btnConnect.Left - cmbSerialPorts.Width - 20;
        }

        private void Form1_Resize(object sender, EventArgs e)
        {
            txtConsole.Width = this.Width - 40;
            txtConsole.Height = this.Height - 90;
            moveSerialPortComboAndConnectButton();
        }

        private string doCompileSync()
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
                    Cursor.Current = Cursors.WaitCursor;
                    string compileErrorMessage = CompilationClient.CompileSync("http://openbrainiacs.com/led_compiler/compile_raw.php", sourceFilename, destinationFilename);
                    Cursor.Current = Cursors.Default;
                    if (compileErrorMessage == null)
                    {
                        return destinationFilename;
                    }
                    else
                    {
                        MessageBox.Show(this, compileErrorMessage, "Compilation Error");
                    }
                }
                catch (Exception ex)
                {
                    Cursor.Current = Cursors.Default;
                    MessageBox.Show(this, ex.Message, "Error");
                }
            }
            return null;
        }

        private void pollSerial()
        {
            if (serialPort.BytesToRead > 0)
            {
                byte[] buffer = new byte[serialPort.ReadBufferSize];
                int bytesRead = serialPort.Read(buffer, 0, buffer.Length);
                if (bytesRead > 0)
                {
                    string s = System.Text.Encoding.UTF8.GetString(buffer, 0, bytesRead);
                    txtConsole.Text += s;
                    //if (txtConsole.Text.Length >= consoleHighWaterMark)                                              // if at high, set to low
                    //{
                    //    txtConsole.Text = txtConsole.Text.Substring(txtConsole.Text.Length - consoleLowWaterMark);
                    //}
                    txtConsole.Select(txtConsole.Text.Length, 0);
                    txtConsole.ScrollToCaret();
                }
            }
        }

        private void compileFileToolStripMenuItem_Click(object sender, EventArgs e)
        {
            string compiledFilename = doCompileSync();
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
                    serialPort.Write(line + "\r");
                    Thread.Sleep(10);
                    pollSerial();
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
            string compiledFilename = doCompileSync();
            if (compiledFilename != null)
            {
                doUpload(compiledFilename);
            }
        }

        private void exitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (connected)
            {
                connected = false;
                serialPort.Close();
            }
            Application.Exit();
        }

        private void aboutToolStripMenuItem_Click(object sender, EventArgs e)
        {
            AboutBox aboutBox = new AboutBox();
            aboutBox.Show();
        }

        private void MainForm_FormClosed(object sender, FormClosedEventArgs e)
        {
            if (connected)
            {
                connected = false;
                serialPort.Close();
            }
        }
    }
}
