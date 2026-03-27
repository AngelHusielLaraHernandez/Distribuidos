from multiprocessing import process, Pipe
from os import getpid
from datetime  import datetime

def local_time (counter):
    return '(LAMPORT_TIME={}, LOCAL_TIME={}) '.format(counter, datetime.now())

def calc_rev_timestamp(recev_time_stamp, local_counter):
    return max(recev_time_stamp, local_counter) + 1

def event(pid, counter):
    counter += 1
    print("Algo sucedio en el proceso {} !".format(pid)+ local_time(counter))
    return counter
def send_message(pipe, pid, counter):
    counter += 1
    pipe.send(('Mensaje', counter))
    print ("Proceso {} envio un mensaje !".format(pid)+ local_time(counter))
    return counter

def recv_message(pipe, pid, counter):
    message, timestamp = pipe.recv()
    counter = calc_rev_timestamp(timestamp, counter)
    print('Mensaje enviado a '+ str(pid)+ local_time(counter))
    return counter

def process_one(pipe12):
    pid=getpid()
    counter=0
    counter=event(pid, counter)
    counter=send_message(pipe12, pid, counter)
    counter=event(pid, counter)
    counter=recv_message(pipe12, pid, counter)
    counter=event(pid, counter)
    
def process_two(pipe21, pipe23):
    pid= getpid()
    counter=0
    counter=recv_message(pipe21, pid, counter)
    counter=send_message(pipe21, pid, counter)
    counter=recv_message(pipe23, pid, counter)
    counter=send_message(pipe23, pid, counter)
    
def process_three(pipe32):
    pid= getpid()
    counter=0
    counter=recv_message(pipe32, pid, counter)
    counter=send_message(pipe32, pid, counter)

if __name__ == '__main__':
    pipe12, pipe21 = Pipe()
    pipe23, pipe32 = Pipe()
    
    p1= process(target=process_one, args=(pipe12,))
    p2= process(target=process_two, args=(pipe21, pipe23))
    p3= process(target=process_three, args=(pipe32,))
    
    p1.start()
    p2.start()
    p3.start()
    
    p1.join()
    p2.join()
    p3.join()