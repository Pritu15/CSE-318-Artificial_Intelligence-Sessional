


let redExists = false, blueExists = false;

function getCriticalMass(row, col) {
    let cnt = 0;
    if (row > 0) cnt++;
    if (row < 8) cnt++;
    if (col > 0) cnt++;
    if (col < 5) cnt++;
    return cnt;
}

function explode(board, row, col, player) {
    const queue = [[row, col]];

    while (queue.length > 0) {
        const [r, c] = queue.shift();
        const criticalMass = getCriticalMass(r, c);

        if (board[r][c].orbs < criticalMass) continue;

        // Clear current cell
        board[r][c].orbs -= criticalMass;
        if (board[r][c].orbs === 0) {
            board[r][c].playerType = '0';
        }

        // Spread to neighbors
        const directions = [[-1, 0], [1, 0], [0, -1], [0, 1]];
        for (const [dr, dc] of directions) {
            const nr = r + dr, nc = c + dc;
            if (nr < 0 || nr >= 9 || nc < 0 || nc >= 6) continue;

            board[nr][nc].orbs++;
            board[nr][nc].playerType = player;

            if (board[nr][nc].orbs >= getCriticalMass(nr, nc)) {
                queue.push([nr, nc]);
            }
        }
    }
}

function isMoveApplied(board, row, col, player) {
    if (board[row][col].playerType !== '0' && board[row][col].playerType !== player) {
        return false;
    }

    board[row][col].orbs++;
    board[row][col].playerType = player;

    if (board[row][col].orbs >= getCriticalMass(row, col)) {
        explode(board, row, col, player);
    }

    return true;
}

function isWinning(board) {
    let redOrbs = 0, blueOrbs = 0;
    let winner = null;
    console.log('The redExits', redExists, 'blueExists', blueExists);
    for (const row of board) {
        for (const cell of row) {
            if (cell.playerType === 'R') {
                redOrbs += cell.orbs;
                redExists = true;
            } else if (cell.playerType === 'B') {
                blueOrbs += cell.orbs;
                blueExists = true;
            }
        }
    }

    if (redOrbs > 0 && blueOrbs === 0 && blueExists) {
        winner = 'R';
        console.log('The winner is', winner);
        return { isWin: true, winner };
    }
    if (blueOrbs > 0 && redOrbs === 0 && redExists) {
        winner = 'B';
        console.log('The winner is', winner);
        return { isWin: true, winner };
    }

    return { isWin: false, winner: null };
}

function getValidMoves(board, player) {
    const moves = [];
    for (let r = 0; r < 9; r++) {
        for (let c = 0; c < 6; c++) {
            if (board[r][c].playerType === '0' || board[r][c].playerType === player) {
                moves.push([r, c]);
            }
        }
    }
    return moves;
}

export { getCriticalMass, explode, isMoveApplied, isWinning, getValidMoves };