import diane

config = diane.getConfig('FileTransfer')

config.addOption('DEFAULT_CHUNK_SIZE',100000,'default chunk size for file transfer')
config.addOption('ORPHANED_SESSION_TIMEOUT',200,'if transfer of one chunk takes longer than this timeout then the session is cancelled')
