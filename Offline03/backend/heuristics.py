# === backend/heuristics.py ===
def orb_count(board, player):
    return sum(cell['orbs'] for row in board for cell in row if cell['player'] == player)

def mobility(board, player):
    return sum(1 for r in range(9) for c in range(6) if board[r][c]['player'] in [None, player])

def stability(board, player):
    from game import get_critical_mass
    return sum(1 for r in range(9) for c in range(6)
               if board[r][c]['player'] == player and board[r][c]['orbs'] < get_critical_mass(r, c)-1)

def critical_threat(board, player):
    from game import get_critical_mass
    opp = 'red' if player == 'blue' else 'blue'
    threats = 0
    for r in range(9):
        for c in range(6):
            if board[r][c]['player'] == player and board[r][c]['orbs'] == get_critical_mass(r, c)-1:
                for dr, dc in [(-1,0), (1,0), (0,-1), (0,1)]:
                    nr, nc = r + dr, c + dc
                    if 0 <= nr < 9 and 0 <= nc < 6 and board[nr][nc]['player'] == opp:
                        threats += 1
    return threats

def weighted_position(board, player):
    weights = [[3,4,5,5,4,3], [4,6,8,8,6,4], [5,8,10,10,8,5], [5,8,10,10,8,5],
               [5,8,10,10,8,5], [5,8,10,10,8,5], [5,8,10,10,8,5], [4,6,8,8,6,4], [3,4,5,5,4,3]]
    return sum(weights[r][c] for r in range(9) for c in range(6) if board[r][c]['player'] == player)

HEURISTICS = {
    'orb_count': orb_count,
    'mobility': mobility,
    'stability': stability,
    'critical_threat': critical_threat,
    'weighted_position': weighted_position
}
