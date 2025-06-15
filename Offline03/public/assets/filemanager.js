// utils/filemanager.js
const fs = require('fs');
const path = require('path');

const GAMESTATE_FILE = path.join(__dirname, '../gamestate.txt');

function readGameState() {
    try {
        const data = fs.readFileSync(GAMESTATE_FILE, 'utf8');
        const lines = data.trim().split('\n');
        const header = lines[0].trim();
        const board = lines.slice(1).map(line => 
            line.trim().split(/\s+/).map(cell => {
                if (cell === '0') return { orbs: 0, playerType: '0' };
                return { orbs: parseInt(cell[0]), playerType: cell[1] };
            })
        );
        return { header, board };
    } catch (err) {
        if (err.code === 'ENOENT') {
            // File doesn't exist yet - initialize it
            const initialBoard = Array(9).fill().map(() => 
                Array(6).fill().map(() => ({ orbs: 0, playerType: '0' }))
            );
            writeGameState('Human Move:', initialBoard);
            return { header: 'Human Move:', board: initialBoard };
        }
        throw err;
    }
}

function writeGameState(header, board) {
    const boardStr = board.map(row => 
        row.map(cell => 
            cell.orbs === 0 ? '0' : `${cell.orbs}${cell.playerType}`
        ).join(' ')
    ).join('\n');
    
    fs.writeFileSync(GAMESTATE_FILE, `${header}\n${boardStr}`);
}

function waitForTurn(expectedHeader, timeout=5000) {
    const start = Date.now();
    while (Date.now() - start < timeout) {
        const state = readGameState();
        if (state && state.header === expectedHeader) {
            return state;
        }
        // Poll every 100ms
        require('deasync').sleep(100);
    }
    throw new Error(`Timeout waiting for ${expectedHeader}`);
}

module.exports = { readGameState, writeGameState, waitForTurn };