# === backend/game.py ===
def get_critical_mass(row, col):
    neighbors = 0
    for dr, dc in [(-1,0), (1,0), (0,-1), (0,1)]:
        if 0 <= row + dr < 9 and 0 <= col + dc < 6:
            neighbors += 1
    return neighbors

def explode(board, row, col):
    queue = [(row, col)]
    while queue:
        r, c = queue.pop(0)
        critical = get_critical_mass(r, c)
        if board[r][c]['orbs'] < critical:
            continue
        player = board[r][c]['player']
        board[r][c]['orbs'] -= critical
        if board[r][c]['orbs'] == 0:
            board[r][c]['player'] = None
        for dr, dc in [(-1,0), (1,0), (0,-1), (0,1)]:
            nr, nc = r + dr, c + dc
            if 0 <= nr < 9 and 0 <= nc < 6:
                board[nr][nc]['orbs'] += 1
                board[nr][nc]['player'] = player
                if board[nr][nc]['orbs'] >= get_critical_mass(nr, nc):
                    queue.append((nr, nc))

def apply_human_move(board, row, col):
    if board[row][col]['player'] not in [None, 'red']:
        return board, False
    board[row][col]['orbs'] += 1
    board[row][col]['player'] = 'red'
    explode(board, row, col)
    return board, True

def apply_ai_move(board):
    from minimax import minimax_search
    _, action = minimax_search(board, depth=4, player='blue')
    r, c = action
    board[r][c]['orbs'] += 1
    board[r][c]['player'] = 'blue'
    explode(board, r, c)
    return board