from multiprocessing import Process, Pipe
from os import getpid
from datetime import datetime

# ==========================================
# CÓDIGOS DE COLOR ANSI PARA LA TERMINAL
# ==========================================
RESET = "\033[0m"
C_EVENT = "\033[93m"  # Amarillo
C_SEND  = "\033[92m"  # Verde
C_RECV  = "\033[96m"  # Cian
C_TIME  = "\033[90m"  # Gris

def local_time(counter):
    time_str = datetime.now().strftime('%H:%M:%S.%f')[:-3]
    # Se formatea el arreglo vectorial para que todos tengan el mismo espacio
    vector_str = f"[{', '.join(f'{x:02d}' for x in counter)}]"
    return f'{C_TIME}[ Reloj Vectorial: {vector_str} | Hora: {time_str} ]{RESET}'

def calc_recv_timestamp(recv_time_stamp, counter):
    return [max(r, c) for r, c in zip(recv_time_stamp, counter)]

def event(pid, p_index, counter):
    counter[p_index] += 1
    accion = f'{C_EVENT}[P{p_index}] EVENTO LOCAL   (PID:{pid}){RESET}'.ljust(52)
    print(f'{accion} {local_time(counter)}')
    return counter

def send_message(pipe, pid, p_index, counter, dest_num):
    counter[p_index] += 1
    pipe.send(('Mensaje', counter))
    accion = f'{C_SEND}[P{p_index}] ENVÍA a P{dest_num}    (PID:{pid}){RESET}'.ljust(52)
    print(f'{accion} {local_time(counter)}')
    return counter

def recv_message(pipe, pid, p_index, counter, src_num):
    message, timestamp = pipe.recv()
    counter = calc_recv_timestamp(timestamp, counter)
    counter[p_index] += 1
    accion = f'{C_RECV}[P{p_index}] RECIBE de P{src_num}  (PID:{pid}){RESET}'.ljust(52)
    print(f'{accion} {local_time(counter)}')
    return counter

# ==========================================
# DEFINICIÓN DE LOS 5 PROCESOS
# ==========================================
def p0(pipe01, pipe02):
    pid = getpid()
    counter = [0, 0, 0, 0, 0]
    counter = event(pid, 0, counter)
    counter = send_message(pipe01, pid, 0, counter, 1)
    counter = send_message(pipe02, pid, 0, counter, 2)

def p1(pipe10, pipe13):
    pid = getpid()
    counter = [0, 0, 0, 0, 0]
    counter = recv_message(pipe10, pid, 1, counter, 0)
    counter = send_message(pipe13, pid, 1, counter, 3)

def p2(pipe20, pipe24):
    pid = getpid()
    counter = [0, 0, 0, 0, 0]
    counter = recv_message(pipe20, pid, 2, counter, 0)
    counter = send_message(pipe24, pid, 2, counter, 4)

def p3(pipe31, pipe34):
    pid = getpid()
    counter = [0, 0, 0, 0, 0]
    counter = recv_message(pipe31, pid, 3, counter, 1)
    counter = send_message(pipe34, pid, 3, counter, 4)

def p4(pipe42, pipe43):
    pid = getpid()
    counter = [0, 0, 0, 0, 0]
    counter = recv_message(pipe42, pid, 4, counter, 2)
    counter = recv_message(pipe43, pid, 4, counter, 3)
    counter = event(pid, 4, counter)

if __name__ == '__main__':
    print("\n" + "="*80)
    print(" INICIANDO SIMULACIÓN DE RELOJES VECTORIALES (5 PROCESOS) ".center(80, "="))
    print("="*80 + "\n")

    pipe01, pipe10 = Pipe()
    pipe02, pipe20 = Pipe()
    pipe13, pipe31 = Pipe()
    pipe24, pipe42 = Pipe()
    pipe34, pipe43 = Pipe()

    procesos = [
        Process(target=p0, args=(pipe01, pipe02)),
        Process(target=p1, args=(pipe10, pipe13)),
        Process(target=p2, args=(pipe20, pipe24)),
        Process(target=p3, args=(pipe31, pipe34)),
        Process(target=p4, args=(pipe42, pipe43))
    ]

    for p in procesos:
        p.start()
    for p in procesos:
        p.join()
        
    print("\n" + "="*80)
    print(" SIMULACIÓN FINALIZADA ".center(80, "="))
    print("="*80 + "\n")