import grpc
import calculadora_pb2
import calculadora_pb2_grpc

def run():

    channel = grpc.insecure_channel('localhost:50051')
    
    stub = calculadora_pb2_grpc.CalculadoraStub(channel)


    a = int(input("Ingrese el primer número: "))
    b = int(input("Ingrese el segundo número: "))
    
    request = calculadora_pb2.SumaRequest(a=a, b=b)


    response = stub.Sumar(request)

    print(f'El resultado de la suma es: {response.resultado}')


    request_mult = calculadora_pb2.SumaRequest(a=a, b=b)
    response_mult = stub.Multi(request_mult)
    
    print(f'El resultado de la multiplicación es: {response_mult.resultado}')
    

if __name__ == '__main__':
    run()