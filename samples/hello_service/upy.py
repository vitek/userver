import time


def init(idx):
    print(f'I am interpreter {idx}')

    def script():
        for _ in range(10000):
            pass
        return f'Hello from python interpreter {idx}\n'

    return script

