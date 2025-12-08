Including Options in the APL Command
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    You can include options in the APL command when
    you bring up the system. The complete format of the
    APL command is:

APL [EXAPL] [APx] [APy] ... [APz] [APL system command]

    where the maximum number of names given after APL,
    excluding the field, "APL system command," is six.

    • EXAPL is the filename specification of the program
      that has the dyadic formats (numeric format and
      picture format). For more information, see
        Chapter 7.

    • APx, APy, and APz represent the filenames of
      auxiliary processors, which are programs that carry
      out special actions not included in the APL
      language. You can also build your own auxiliary
      processors (see Chapter 4). The following auxiliary
      processors are included with the system:

      - AP80: IBM Graphics Printer control
      - AP 1 00: BIOSIDOS interrupt handling
      - AP205: Full-screen display management
      - AP210: DOS file management
      - AP232: Asynchronous communications
      - AP440: Music generator

    • "APL system command" means that you can type
      here any APL system command to be executed at
      load time, thus giving you the possibility of
      automatically starting an APL application. (For the
      syntax of APL system commands, see Chapter 12.)
      This field, if given, must always be the last one in
      the line, and it must start with a right parenthesis.
      All letters must be uppercase.

    If you wish to always have EXAPL or some auxiliary
    processor support, or automatically execute an APL
    system command, you may create a batch file to do so
    (see your IBM Personal Computer DOS manual).

    The APL system command, `)OFF`, is used to exit from
    an APL work session and transfer control to DOS. The
    active workspace is lost unless it was explicitly stored
    earlier in the work session with a `)SAVE` or `)OUT`
    command. Any variables actively shared with an
    auxiliary processor will be automatically retracted upon
    exit from the APL system.

    Examples:

    APL AP210 AP100 )LOAD WORKSP

    This starts APL and auxiliary processors AP210 and
    AP100. Also, the workspace called WORKSP is
    loaded. The Graphics Printer will not be available.

    APL EXAPL AP80

    This will start APL with the dyadic formats in EXAPL.
    The printer is available.
