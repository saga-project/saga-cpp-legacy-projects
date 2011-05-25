from optparse import OptionParser

def _expand_prog_name(parser,text):
    try:
        return parser.expand_prog_name(text)
    except AttributeError: #2.3 compatibility
        return text.replace("%prog", parser._get_prog_name())

def _addnewline(s):
    if s[-1] != '\n':
        return s+'\n'
    return s

class ExtendedOptionParser(OptionParser):
    def __init__(self, subparsers, short_command_summary=True,strict_commands=True,**kwds):
        """ CVS-style command line parsing: prog general-options command command-options arguments

        Commands' syntax is defined by OptionParser instances in subparsers list (prog attribute is a command name).

        If short_command_summary is True then prog -h gives only a brief command description instead of full syntax.
        If strict_commands is True than the command string must match one of the subparsers.
        """
        OptionParser.__init__(self, **kwds)
        self.subparsers = {}
        for subparser in subparsers:
            if self.subparsers.has_key(subparser.prog):
                raise Error('multiply defined command %s'%subparser.prog)
            self.subparsers[subparser.prog] = subparser
        self.short_command_summary=short_command_summary
        self.strict_commands = strict_commands
        self.command=None
        self.disable_interspersed_args()

    def format_help(self, formatter=None):
        res = OptionParser.format_help(self,formatter)
        res += "\nCommands:\n"
        for subparser in self.subparsers.values():
            if formatter is None:
                subformatter = subparser.formatter
            else:
                subformatter = formatter

            if self.short_command_summary:
                res += _addnewline(subformatter.format_description(_expand_prog_name(subparser,subparser.description.splitlines()[0])))
            else:
                res += '\n'
                res += subparser.format_help(subformatter)

        if self.short_command_summary:
            res += _expand_prog_name(self,'\nGet help on a specific command: %prog command -h\n')
        return res

    def parse_args(self, args=None, values=None):
        (options,args) = OptionParser.parse_args(self, args, values)
        try:
            subparser = self.subparsers[args[0]]
            self.command = args[0]
        except KeyError,x:
            if self.strict_commands:
                self.error('command %s undefined'%args[0])
            else:
                self.command = args[0]
                return (options,args)
        except IndexError,x:
            return (options,args)
        
        subparser.parse_args(args[1:])
        return (options,args)
        
if __name__ == '__main__':

    print_cmd = OptionParser(prog="print",description="%prog: print stuff.\nThis command will print a lot of spam\n...and spam\n...and spam")
    print_cmd.add_option("-q","--quiet",action="store_false",dest="verbose",default=True,help="print less messages")
    print_cmd.usage=''

    validate_cmd = OptionParser(prog="validate",description="%prog: validate stuff")
    validate_cmd.add_option("-q","--quiet",action="store_false",dest="verbose",default=True,help="print less messages")
    validate_cmd.usage=''
    
    parser = ExtendedOptionParser([print_cmd,validate_cmd],short_command_summary=True,usage="%prog [options] command [cmdopts] ",
                                  description="CVS-style option parsing example")

    parser.add_option("-v", "--verbose", action="store_true",dest="verbose", default=False, help="print more messages")

    (options, args) = parser.parse_args()
    
    print "Here's what I found out:"
    print "General options:",options
    print "General args:",args
    print "Current command:",parser.command
    if parser.command:
        print "Command specific options:", parser.subparsers[parser.command].values
        print "Command specific arguments:", parser.subparsers[parser.command].largs+parser.subparsers[parser.command].rargs

