def init(idx):
    print(f'I am interpreter {idx}')

    def script():
        return f'Hello from python interpreter {idx}\n'

    return script

