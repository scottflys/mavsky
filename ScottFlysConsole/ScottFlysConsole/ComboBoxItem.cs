using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ScottFlysConsole
{
    class ComboBoxItem
    {
        string displayValue;
        string hiddenValue;

        public ComboBoxItem(string hiddenValue, string displayValue)
        {
            this.hiddenValue = hiddenValue;
            this.displayValue = displayValue;
        }

        public string HiddenValue
        {
            get
            {
                return hiddenValue;
            }
        }

        public override string ToString()
        {
            return displayValue;
        }
    }
}
