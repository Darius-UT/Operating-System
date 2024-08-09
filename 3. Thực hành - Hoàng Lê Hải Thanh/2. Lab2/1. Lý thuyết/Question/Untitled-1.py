# """ Thư viện cho biết thời gian thực thi của chương trình """
import time 

# """ Thư viện hỗ trợ multithreading """
import threading

# """ Hàm tính diện tích hình vuông """
def calc_square(numbers):
    for n in numbers:
        print(f'\n{n} ^ 2 = {n*n}')
        time.sleep(0.1)
        # cho ngủ 0.1 giây

# """ Hàm tính thể tích hình lập phương """
def calc_cube(numbers):
    for n in numbers:
        print(f'\n{n} ^ 3 = {n*n*n}')
        time.sleep(0.1)

numbers = [2, 3, 5, 8]
start = time.time()

# """ Thread 1: xử lý hàm calc_square"""
square_thread = threading.Thread(target=calc_square, args=(numbers,))
# """ Thread 2 xử lý hàm calc_cube"""
cube_thread = threading.Thread(target=calc_cube, args=(numbers,))

# """ Gọi thread """
square_thread.start()
cube_thread.start()

square_thread.join()
cube_thread.join()

end = time.time()

print('Execution Time: {}'.format(end-start))
