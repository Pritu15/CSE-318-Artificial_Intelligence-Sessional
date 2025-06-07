# === backend/minimax.py ===
from heuristics import HEURISTICS
import copy

def get_critical_mass(row, col):
    neighbors = 0
    for dr, dc in [(-1,0), (1,0), (0,-1), (0,1)]:
        if 0 <= row + dr < 9 and 0 <= col + dc < 6:
            neighbors += 1
    return neighbors

def get_valid_moves(board, player):
    return [(r, c) for r in range(9) for c in range(6) if board[r][c]['player'] in [None, player]]

def evaluate(board, heuristic, player):
    return HEURISTICS[heuristic](board, player) - HEURISTICS[heuristic](board, 'red' if player == 'blue' else 'blue')

def minimax_search(board, depth, player, alpha=float('-inf'), beta=float('inf'), maximizing=True, heuristic='weighted_position'):
    opponent = 'red' if player == 'blue' else 'blue'
    valid_moves = get_valid_moves(board, player if maximizing else opponent)

    if depth == 0 or not valid_moves:
        return evaluate(board, heuristic, player), None

    best_move = None

    if maximizing:
        max_eval = float('-inf')
        for move in valid_moves:
            new_board = copy.deepcopy(board)
            r, c = move
            new_board[r][c]['orbs'] += 1
            new_board[r][c]['player'] = player
            from game import explode
            explode(new_board, r, c)
            eval, _ = minimax_search(new_board, depth-1, player, alpha, beta, False, heuristic)
            if eval > max_eval:
                max_eval = eval
                best_move = move
            alpha = max(alpha, eval)
            if beta <= alpha:
                break
        return max_eval, best_move

    else:
        min_eval = float('inf')
        for move in valid_moves:
            new_board = copy.deepcopy(board)
            r, c = move
            new_board[r][c]['orbs'] += 1
            new_board[r][c]['player'] = opponent
            from game import explode
            explode(new_board, r, c)
            eval, _ = minimax_search(new_board, depth-1, player, alpha, beta, True, heuristic)
            if eval < min_eval:
                min_eval = eval
                best_move = move
            beta = min(beta, eval)
            if beta <= alpha:
                break
        return min_eval, best_move
