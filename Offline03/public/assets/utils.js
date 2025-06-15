import fs from 'fs';

class Cell {
    constructor(orbs = 0, playerType = '0') {
        this.orbs = orbs;
        this.playerType = playerType;
    }
}

function parseCell(cellStr) {
    if (cellStr === '0') return new Cell();
    return new Cell(
        parseInt(cellStr.slice(0, -1)), // orbs (all characters except last)
        cellStr.slice(-1) // playerType (last character)
    );
}

function stringifyCell(cell) {
    return cell.orbs === 0 ? '0' : `${cell.orbs}${cell.playerType}`;
}

function readGamestateFromFile(filename = 'gamestate.txt') {
    try {
        const data = fs.readFileSync(filename, 'utf8');
        const lines = data.trim().split('\n');
        
        // First line is the header
        const header = lines[0].trim();
        
        // Parse board (next 9 lines)
        const board = lines.slice(1, 10).map(line => 
            line.trim().split(/\s+/).map(parseCell)
        );
        
        return { header, board };
    } catch (err) {
        console.error("Error reading gamestate file:", err);
        
        // Return default state if file doesn't exist
        if (err.code === 'ENOENT') {
            const defaultBoard = Array(9).fill().map(() => 
                Array(6).fill().map(() => new Cell())
            );
            return {
                header: "Human Move:",
                board: defaultBoard
            };
        }
        
        throw err;
    }
}

function writeGamestateToFile(filename = 'gamestate.txt', header, board) {
    try {
        // Convert board to text format
        const boardText = board.map(row => 
            row.map(stringifyCell).join(' ')
        ).join('\n');
        
        // Write header + board to file
        fs.writeFileSync(filename, `${header}\n${boardText}`);
        return true;
    } catch (err) {
        console.error("Error writing gamestate file:", err);
        return false;
    }
}

// Helper function to wait for specific header
function waitForHeader(filename, expectedHeader, timeout = 5000) {
    const start = Date.now();
    while (Date.now() - start < timeout) {
        const { header } = readGamestateFromFile(filename);
        if (header === expectedHeader) {
            return true;
        }
        // Sleep for 100ms before checking again
        Atomics.wait(new Int32Array(new SharedArrayBuffer(4)), 0, 0, 100);
    }
    throw new Error(`Timeout waiting for header: ${expectedHeader}`);
}

export { 
    Cell,
    readGamestateFromFile, 
    writeGamestateToFile,
    waitForHeader
};