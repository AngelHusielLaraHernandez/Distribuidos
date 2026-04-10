from multiprocessing import Process, Pipe
from os import getpid
from datetime import datetime

# ==========================================
# CÓDIGOS DE COLOR ANSI PARA LA TERMINAL
# ==========================================
RESET = "\033[0m"
C_EVENT = "\033[93m"  # Amarillo para eventos locales
C_SEND  = "\033[92m"  # Verde para envíos
C_RECV  = "\033[96m"  # Cian para recepciones
C_TIME  = "\033[90m"  # Gris para los relojes

def local_time(counter):
    # Formateamos la hora para que sea más limpia (HH:MM:SS.mmm)
    time_str = datetime.now().strftime('%H:%M:%S.%f')[:-3]
    return f'{C_TIME}[ Reloj Lamport: {counter:02d} | Hora: {time_str} ]{RESET}'

def calc_recv_timestamp(recv_time_stamp, counter):
    return max(recv_time_stamp, counter) + 1

def event(pid, counter, p_num):
    counter += 1
    # Se ajusta el espaciado (ljust) para crear un efecto de tabla
    accion = f'{C_EVENT}[P{p_num}] EVENTO LOCAL   (PID:{pid}){RESET}'.ljust(52)
    print(f'{accion} {local_time(counter)}')
    return counter

def send_message(pipe, pid, counter, p_num, dest_num):
    counter += 1
    pipe.send(('Mensaje', counter))
    accion = f'{C_SEND}[P{p_num}] ENVÍA a P{dest_num}    (PID:{pid}){RESET}'.ljust(52)
    print(f'{accion} {local_time(counter)}')
    return counter

def recv_message(pipe, pid, counter, p_num, src_num):
    message, timestamp = pipe.recv()
    counter = calc_recv_timestamp(timestamp, counter)
    accion = f'{C_RECV}[P{p_num}] RECIBE de P{src_num}  (PID:{pid}){RESET}'.ljust(52)
    print(f'{accion} {local_time(counter)}')
    return counter

# ==========================================
# DEFINICIÓN DE LOS 7 PROCESOS
# ==========================================
def p0(pipe01):
    pid = getpid()
    counter = 0
    counter = event(pid, counter, 0)
    counter = send_message(pipe01, pid, counter, 0, 1)

def p1(pipe10, pipe12, pipe13):
    pid = getpid()
    counter = 0
    counter = recv_message(pipe10, pid, counter, 1, 0)
    counter = event(pid, counter, 1)
    counter = send_message(pipe12, pid, counter, 1, 2)
    counter = send_message(pipe13, pid, counter, 1, 3)

def p2(pipe21, pipe24):
    pid = getpid()
    counter = 0
    counter = recv_message(pipe21, pid, counter, 2, 1)
    counter = send_message(pipe24, pid, counter, 2, 4)

def p3(pipe31, pipe35):
    pid = getpid()
    counter = 0
    counter = recv_message(pipe31, pid, counter, 3, 1)
    counter = send_message(pipe35, pid, counter, 3, 5)

def p4(pipe42, pipe46):
    pid = getpid()
    counter = 0
    counter = recv_message(pipe42, pid, counter, 4, 2)
    counter = send_message(pipe46, pid, counter, 4, 6)

def p5(pipe53, pipe56):
    pid = getpid()
    counter = 0
    counter = recv_message(pipe53, pid, counter, 5, 3)
    counter = send_message(pipe56, pid, counter, 5, 6)

def p6(pipe64, pipe65):
    pid = getpid()
    counter = 0
    counter = recv_message(pipe64, pid, counter, 6, 4)
    counter = recv_message(pipe65, pid, counter, 6, 5)
    counter = event(pid, counter, 6)

if __name__ == '__main__':
    print("\n" + "="*70)
    print(" INICIANDO SIMULACIÓN DE RELOJES DE LAMPORT (7 PROCESOS) ".center(70, "="))
    print("="*70 + "\n")

    # Creación de tuberías para comunicación
    pipe01, pipe10 = Pipe()
    pipe12, pipe21 = Pipe()
    pipe13, pipe31 = Pipe()
    pipe24, pipe42 = Pipe()
    pipe35, pipe53 = Pipe()
    pipe46, pipe64 = Pipe()
    pipe56, pipe65 = Pipe()

    # Inicialización
    procesos = [
        Process(target=p0, args=(pipe01,)),
        Process(target=p1, args=(pipe10, pipe12, pipe13)),
        Process(target=p2, args=(pipe21, pipe24)),
        Process(target=p3, args=(pipe31, pipe35)),
        Process(target=p4, args=(pipe42, pipe46)),
        Process(target=p5, args=(pipe53, pipe56)),
        Process(target=p6, args=(pipe64, pipe65))
    ]

    for p in procesos:
        p.start()
    for p in procesos:
        p.join()
        
    print("\n" + "="*70)
    print(" SIMULACIÓN FINALIZADA ".center(70, "="))
    print("="*70 + "\n")