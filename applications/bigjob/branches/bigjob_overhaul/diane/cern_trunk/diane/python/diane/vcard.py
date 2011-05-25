def make_vcard(extras = None):
    import os

    vcard = {}

    vcard['ERRORS'] = {}

    try:
        import socket
        vcard['HOSTNAME'] = socket.gethostbyaddr(socket.gethostname())[0]
        # while working offline and with an improper /etc/hosts configuration
        # the localhost cannot be resolved
    except Exception,x:
        vcard['HOSTNAME'] = ''
        vcard['ERRORS']['HOSTNAME'] = str(x)

    vcard['ENVIRONMENT'] = os.environ

    def readf(what,fn):
        try:
            vcard[what] = file(fn).read()
        except Exception,x:
            vcard[what] = ''
            vcard['ERRORS'][what] = str(x)

    readf('CPUINFO','/proc/cpuinfo')
    readf('MEMINFO','/proc/meminfo')

    if extras:
        vcard.update(extras)
            
    return vcard


