import { getValidMoves, getCriticalMass } from './board.js';

function heuristicOrbCount(board, player) {
    let score = 0;
    const opponent = player === 'R' ? 'B' : 'R';
    for (const row of board) {
        for (const cell of row) {
            if (cell.playerType === player) score += cell.orbs;
            else if (cell.playerType === opponent) score -= cell.orbs;
        }
    }
    return score;
}

function heuristicMobility(board, player) {
    const playerMoves = getValidMoves(board, player).length;
    const opponent = player === 'R' ? 'B' : 'R';
    const opponentMoves = getValidMoves(board, opponent).length;
    return playerMoves - opponentMoves;
}

function heuristicCriticalCells(board, player) {
    let score = 0;
    for (let r = 0; r < 9; r++) {
        for (let c = 0; c < 6; c++) {
            const cell = board[r][c];
            if (cell.playerType !== player) continue;
            const critical = getCriticalMass(r, c);
            if (cell.orbs === critical - 1) score += 5;
            else if (cell.orbs === critical - 2) score += 2;
        }
    }
    return score;
}

function heuristicStability(board, player) {
    let score = 0;
    for (let r = 0; r < 9; r++) {
        for (let c = 0; c < 6; c++) {
            const cell = board[r][c];
            if (cell.playerType !== player) continue;
            if ((r === 0 || r === 8) && (c === 0 || c === 5)) score += 4;
            else if (r === 0 || r === 8 || c === 0 || c === 5) score += 2;
            else score += 1;
        }
    }
    return score;
}

function heuristicVulnerability(board, player) {
    let penalty = 0;
    const opponent = player === 'R' ? 'B' : 'R';
    const directions = [[-1, 0], [1, 0], [0, -1], [0, 1]];

    for (let r = 0; r < 9; r++) {
        for (let c = 0; c < 6; c++) {
            const cell = board[r][c];
            if (cell.playerType !== player) continue;

            for (const [dr, dc] of directions) {
                const nr = r + dr, nc = c + dc;
                if (nr < 0 || nr >= 9 || nc < 0 || nc >= 6) continue;
                const neighbor = board[nr][nc];
                if (neighbor.playerType === opponent &&
                    neighbor.orbs >= getCriticalMass(nr, nc) - 1) {
                    penalty += 3;
                }
            }
        }
    }
    return -penalty;
}

function evaluateBoard(board, player) {
    return heuristicOrbCount(board, player) +
           heuristicMobility(board, player) +
           heuristicCriticalCells(board, player) +
           heuristicStability(board, player) +
           heuristicVulnerability(board, player);
}

export { evaluateBoard };