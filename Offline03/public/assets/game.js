import { Cell } from './utils.js';
import { isMoveApplied, isWinning, getValidMoves } from './board.js';
import { minimaxAlgo } from './minimax.js';

class Game {
    constructor() {
        this.gameMode = null;
        this.board = Array(9).fill().map(() => Array(6).fill().map(() => new Cell()));
        this.currentPlayer = 'R';
        this.gameOver = false;
        this.winner = null;
        this.aiThinking = false;
        this.aiMoveDelay = 800; // 0.8 second delay between AI moves
        this.aiTimeout = null;
        this.cells = []; // Store references to all cell elements
        
        this.initControls();
        this.initBoard(); // Initialize DOM elements first
        this.resetGame(); // Then reset game state
    }

    initControls() {
        const changeMode = (mode) => {
            if (this.aiTimeout) {
                clearTimeout(this.aiTimeout);
                this.aiTimeout = null;
            }
            this.setGameMode(mode);
        };

        document.getElementById('human-vs-human').addEventListener('click', () => changeMode('human-vs-human'));
        document.getElementById('human-vs-ai').addEventListener('click', () => changeMode('human-vs-ai'));
        document.getElementById('ai-vs-ai').addEventListener('click', () => changeMode('ai-vs-ai'));
        document.getElementById('restart').addEventListener('click', () => this.resetGame());
    }

    initBoard() {
        const boardElement = document.getElementById('board');
        boardElement.innerHTML = '';
        this.cells = []; // Reset cells array
        
        for (let r = 0; r < 9; r++) {
            const rowElement = document.createElement('div');
            rowElement.className = 'row';
            
            for (let c = 0; c < 6; c++) {
                const cellElement = document.createElement('div');
                cellElement.className = 'cell empty';
                cellElement.dataset.row = r;
                cellElement.dataset.col = c;
                cellElement.addEventListener('click', (e) => this.handleCellClick(e));
                rowElement.appendChild(cellElement);
                this.cells.push(cellElement); // Store reference
            }
            boardElement.appendChild(rowElement);
        }
    }

    setGameMode(mode) {
        if (this.gameMode === mode && !this.gameOver) return;
        
        this.gameMode = mode;
        this.resetGame();
        
        // Update active button styling
        document.querySelectorAll('#game-mode-controls button').forEach(btn => {
            btn.classList.remove('active');
        });
        document.getElementById(mode).classList.add('active');
        
        // Start AI moves if needed
        if (mode === 'ai-vs-ai') {
            this.currentPlayer = 'R'; // Red always starts first
            this.aiTimeout = setTimeout(() => this.makeAIMove(), this.aiMoveDelay);
        } else if (mode === 'human-vs-ai' && this.currentPlayer === 'B') {
            this.aiTimeout = setTimeout(() => this.makeAIMove(), this.aiMoveDelay);
        }
    }

    resetGame() {
        // Clear any pending AI moves
        if (this.aiTimeout) {
            clearTimeout(this.aiTimeout);
            this.aiTimeout = null;
        }

        // Reset board state
        this.board = Array(9).fill().map(() => Array(6).fill().map(() => new Cell()));
        this.currentPlayer = 'R';
        this.gameOver = false;
        this.winner = null;
        this.aiThinking = false;
        
        this.updateBoard();
        this.updateStatus(`Current Player: ${this.currentPlayer === 'R' ? 'Red' : 'Blue'}`);
    }

    handleCellClick(event) {
        if (this.gameOver || this.aiThinking) return;
        
        // Check if human is allowed to move in current mode
        if (this.gameMode === 'ai-vs-ai' || 
           (this.gameMode === 'human-vs-ai' && this.currentPlayer === 'B')) {
            return;
        }
        
        const row = parseInt(event.target.dataset.row);
        const col = parseInt(event.target.dataset.col);
        
        if (isMoveApplied(this.board, row, col, this.currentPlayer)) {
            this.updateBoard();
            this.checkGameStatus();
            
            if (!this.gameOver) {
                this.switchPlayer();
                
                // Trigger AI move if needed
                if ((this.gameMode === 'human-vs-ai' && this.currentPlayer === 'B') || 
                    this.gameMode === 'ai-vs-ai') {
                    this.aiTimeout = setTimeout(() => this.makeAIMove(), this.aiMoveDelay);
                }
            }
        }
    }

    makeAIMove() {
        if (this.gameOver) return;
        
        this.aiThinking = true;
        this.updateStatus(`${this.currentPlayer === 'R' ? 'Red' : 'Blue'} AI is thinking...`);
        
        setTimeout(() => {
            const { move } = minimaxAlgo(this.board, 3, -Infinity, Infinity, true, this.currentPlayer);
            
            if (move[0] !== -1 && move[1] !== -1) {
                isMoveApplied(this.board, move[0], move[1], this.currentPlayer);
                this.updateBoard();
                this.checkGameStatus();
                
                if (!this.gameOver) {
                    this.switchPlayer();
                    
                    // Continue AI moves if in AI vs AI mode
                    if (this.gameMode === 'ai-vs-ai') {
                        this.aiTimeout = setTimeout(() => this.makeAIMove(), this.aiMoveDelay);
                    }
                }
            }
            
            this.aiThinking = false;
        }, 300);
    }

    checkGameStatus() {
        const { isWin, winner } = isWinning(this.board);
        if (isWin) {
            this.gameOver = true;
            this.winner = winner;
            if (this.aiTimeout) {
                clearTimeout(this.aiTimeout);
                this.aiTimeout = null;
            }
            this.updateStatus(`Game Over! ${winner === 'R' ? 'Red' : 'Blue'} wins!`);
            this.updateBoard(); // Show final winning board
        }
    }

    switchPlayer() {
        this.currentPlayer = this.currentPlayer === 'R' ? 'B' : 'R';
        this.updateStatus(`Current Player: ${this.currentPlayer === 'R' ? 'Red' : 'Blue'}`);
    }

    updateBoard() {
        for (let r = 0; r < 9; r++) {
            for (let c = 0; c < 6; c++) {
                const cellIndex = r * 6 + c;
                const cellElement = this.cells[cellIndex];
                const cell = this.board[r][c];
                
                cellElement.className = 'cell';
                cellElement.classList.add(cell.playerType === 'R' ? 'red' : 
                                      cell.playerType === 'B' ? 'blue' : 'empty');
                cellElement.textContent = cell.orbs > 0 ? cell.orbs : '';
            }
        }
    }

    updateStatus(message) {
        document.getElementById('status').textContent = message;
    }
}

// Initialize the game when the DOM is fully loaded
document.addEventListener('DOMContentLoaded', () => {
    new Game();
});