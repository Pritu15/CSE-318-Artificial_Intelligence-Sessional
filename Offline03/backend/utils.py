# === backend/utils.py ===
def read_gamestate(path='gamestate.txt'):
    with open(path, 'r') as f:
        lines = f.read().strip().split('\n')
    header = lines[0].strip()
    board = []
    for line in lines[1:]:
        row = []
        for cell in line.strip().split():
            if cell == '0':
                row.append({'orbs': 0, 'player': None})
            else:
                row.append({'orbs': int(cell[0]), 'player': 'red' if cell[1] == 'R' else 'blue'})
        board.append(row)
    return header, board

def write_gamestate(header, board, path='gamestate.txt'):
    with open(path, 'w') as f:
        f.write(f"{header}\n")
        for row in board:
            line = []
            for cell in row:
                if cell['orbs'] == 0:
                    line.append('0')
                else:
                    p = 'R' if cell['player'] == 'red' else 'B'
                    line.append(f"{cell['orbs']}{p}")
            f.write(" ".join(line) + '\n')