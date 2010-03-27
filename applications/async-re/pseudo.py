"comments: find_partners_exchange() is supposed to be called after the replica are submitted to the bigjob for the first time. It in itself monitors and also finds replicas which are done and matches them with suitable partners to do the exchange. the other methods - search(), select() and update_states_energies() - do the work needed."

def update_states_energies():#gets the states and energies
      for i in number of replicas:
        state[i] = str(sjs[i].get_state())
        energy[i] = str(sjs[i].get_energy())

def search():#searches for other replicas which are in DONE state
   i=0
   while(i< number of replicas) and (i!=x[i]):
      if(state[i]==DONE):
         list.append(i)

def select():#by metropolis scheme select a replica
   for i in list[]:
      if(energy[i] is in required range):
         choose i
   return i

def find_partners_exchange():#once replicas/sjs are submitted, continuosly updates states and energies. as and when a replica is DONE, tries to find a partner for that replica. and then choose a partner and restart that replica. 
   exchange count=0
   while 1:
     i = 0
     for i in number of replicas:
       update_states_energies()
         if(state[i]==DONE):
           x[i]=i #exclude the replica itself when looking for partners
           search()
           if(list[]!=NULL):
             print "partners found"
             select()
             scp the temps to the required dirs
             restart sjs[i]
             count=count+1

