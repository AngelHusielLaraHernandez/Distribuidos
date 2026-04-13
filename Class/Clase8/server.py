from concurrent import futures
import grpc
import calculadora_pb2
import calculadora_pb2_grpc

class Calculadora(calculadora_pb2_grpc.CalculadoraServicer):
    def Sumar(self, request, context):
        a=request.a
        b=request.b
        
        resultado = a + b
        
        return calculadora_pb2.SumaResponse(resultado=resultado)
    def Multi(self, request, context):
        a=request.a
        b=request.b
        
        resultado = a * b
        
        return calculadora_pb2.SumaResponse(resultado=resultado)
def serve():
    server = grpc.server(futures.ThreadPoolExecutor(max_workers=10))
    
    calculadora_pb2_grpc.add_CalculadoraServicer_to_server(
        Calculadora(), 
        server
    )
    
    server.add_insecure_port('[::]:50051')
    server.start()
    
    print("Servidor iniciado en el puerto 50051")
    
    server.wait_for_termination()

if __name__ == '__main__':
    serve()