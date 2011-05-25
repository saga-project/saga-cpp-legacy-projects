def hostname():
    """ Try to get the hostname in the most possible reliable way as described in the Python LibRef."""
    import socket
    try:
        return socket.gethostbyaddr(socket.gethostname())[0]
    # while working offline and with an improper /etc/hosts configuration
    # the localhost cannot be resolved
    except:
        return 'localhost'
