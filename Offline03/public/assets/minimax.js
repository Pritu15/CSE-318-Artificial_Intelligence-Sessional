import { evaluateBoard } from './heuristics.js';
import { isMoveApplied, isWinning, getValidMoves } from './board.js';

function minimaxAlgo(board, depth, alpha, beta, maximizing, aiPlayer) {
    const opponent = aiPlayer === 'R' ? 'B' : 'R';
    const dummy = null;

    const winCheck = isWinning(board);
    if (depth === 0 || winCheck.isWin) {
        return { score: evaluateBoard(board, aiPlayer), move: [-1, -1] };
    }

    const moves = getValidMoves(board, maximizing ? aiPlayer : opponent);
    let bestMove = [-1, -1];
    let bestScore = maximizing ? -Infinity : Infinity;

    for (const move of moves) {
        const newBoard = JSON.parse(JSON.stringify(board));
        isMoveApplied(newBoard, move[0], move[1], maximizing ? aiPlayer : opponent);
        const evalResult = minimaxAlgo(newBoard, depth - 1, alpha, beta, !maximizing, aiPlayer);
        const evalScore = evalResult.score;

        if (maximizing) {
            if (evalScore > bestScore) {
                bestScore = evalScore;
                bestMove = move;
            }
            alpha = Math.max(alpha, evalScore);
        } else {
            if (evalScore < bestScore) {
                bestScore = evalScore;
                bestMove = move;
            }
            beta = Math.min(beta, evalScore);
        }

        if (beta <= alpha) break;
    }

    return { score: bestScore, move: bestMove };
}

export { minimaxAlgo };