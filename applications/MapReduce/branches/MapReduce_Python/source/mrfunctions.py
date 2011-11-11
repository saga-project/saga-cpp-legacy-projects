import saga
import sys
import pdb
import os

class mrfunctions:
    
    def __init__(self,inputdir,tempdir):
        self.__input_dir = inputdir
        self.__tmp_dir = tempdir
        self.__chunksize=64
        self.__chunk_list=[]
        
    def normal_file_chunking(self,chunksize):
        
        """ Split the file and save chunks to separate files """
        self.__chunksize=chunksize
        list_files=[]
        listallfiles=[]
        is_dir = 0
        k = saga.filesystem.file(self.__input_dir)
        if ( k.is_dir() ):
            dirList =  saga.filesystem.directory(self.__input_dir)
            print " Total number of input files in directory " + self.__input_dir + " is " + str(len(dirList.list()))
            list_files =  dirList.list()
            is_dir = 1
        else:
            fname = self.__input_dir
            listallfiles.append(fname)
        if ( k.is_dir()):
            for fname in list_files:
                dname = os.path.split(self.__input_dir)[1]
                bname = os.path.split(self.__input_dir)[0]
                if is_dir :
                    fname = bname + "/" + dname + "/" + str(fname)
                try:
                    f = saga.filesystem.file(fname)
                    if(f.is_dir()):
                        indirList =  saga.filesystem.directory(f.get_url())    
                        inlistfiles = indirList.list()
                        for infile in inlistfiles:
                            listallfiles.append( str(indirList.get_url()) + "/"+  str(infile))
                    else:
                        listallfiles.append(fname)
                except:
                    print " File not opened "
                    sys.exit(0)
                    pass
        privdir=''
        for fname in listallfiles:
            print ' >>> Splitting file - '  + str(fname) + "\n"
            try:
                f = saga.filesystem.file(fname)
                privdir = str(str(fname).replace("//","/")).split("/")
                privdir = privdir[len(privdir)-2]
            except:
                print " File not opened "
                sys.exit(0)
                pass
        
            bname = (os.path.split(str(f.get_url())))[1]

            # Get the file size
            fsize = f.get_size()

            # Get size of each chunk
            self.__numchunks = int(float(fsize)/float(self.__chunksize))
            if int(float(fsize)%float(self.__chunksize)) > 0:
                 self.__numchunks = self.__numchunks + 1
            if self.__numchunks == 0:
                if fsize > 0:
                    self.__numchunks = 1
             
        
            # print " saga file name " + str(f.get_url())
            print " >>> saga file size " + str(f.get_size()) + " >>> Number of chunks " + str(self.__numchunks) + "\n"
            # print ' Number of chunks', self.__numchunks 

    
            chunksz = self.__chunksize
            total_bytes = 0

            for x in range(self.__numchunks):
                chunkfilename = bname + '-' + privdir + '-' + str(x+1) 
                self.__chunk_list.append(self.__tmp_dir + "/" + chunkfilename)

                # if reading the last section, calculate correct
                # chunk size.
                if x == self.__numchunks - 1:
                    chunksz = fsize - total_bytes

                try:
                    print 'Writing file',chunkfilename
                    data = f.read(chunksz)
                    total_bytes += len(data)
                    """ Advert entry has to be created here instead of local chunks """
                    k = self.__tmp_dir + "/" + chunkfilename
                    chunkf = saga.filesystem.file(self.__tmp_dir + "/" + chunkfilename, saga.filesystem.Write)
                    chunkf.write(data)
                    chunkf.close()
                except (OSError, IOError), e:
                    print e
                    continue
                except EOFError, e:
                    print e
                    break
        
        print " \n ***************************** Input File Split is done in temp location... " + self.__tmp_dir + "************************** \n"
        return self.__chunk_list

    def fastq_chunk(self,lines):
        chunk_list=[]
        group_chunks={}
        input=saga.url(self.__input_dir).path
        temp=saga.url(self.__tmp_dir).path
        dirList=os.listdir(input)
        for fname in dirList:
            os.system("cd " + temp + "; split -d -a 5 -l " + str(lines) + " " +  input + "/" + fname + " " + fname + "--" )
        dirList=os.listdir(temp)
        for fname in dirList:
            chunk_list.append(temp + "/" + fname)
        for chunk in chunk_list:
            seq=chunk.split("--")[1]
            if group_chunks.has_key(seq):
               group_chunks[seq].append(chunk)
            else:
               group_chunks[seq] = chunk.split()
        self.__chunk_list = group_chunks.values()   
        return self.__chunk_list

    def chunk_size(self,chunksize):
        self.__chunksize = chunksize
        chunk_list=[]
        group_chunks={}
        input=saga.url(self.__input_dir).path
        temp=saga.url(self.__tmp_dir).path
        dirList=os.listdir(input)
        for fname in dirList:
            os.system("cd " + temp + "; split -d -a 5 -b " + str(chunksize) + " " +  input + "/" + fname + " " + fname + "--" )
        dirList=os.listdir(temp)
        for fname in dirList:
            chunk_list.append([temp + "/" + fname])
        return chunk_list
