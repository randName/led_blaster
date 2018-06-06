from .app import setup, loop

arduinos = []

print('initializing blaster ...')
setup(arduinos)
print(arduinos)

print('blasting ...')
loop(arduinos)
