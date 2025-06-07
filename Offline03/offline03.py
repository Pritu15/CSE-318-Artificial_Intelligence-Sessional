import pygame
import sys
import numpy as np
import time
import os
from enum import Enum
import math
import random
import copy

# Define colors
RED = (255, 0, 0)
BLUE = (0, 0, 255)
WHITE = (255, 255, 255)
BLACK = (0, 0, 0)
GRAY = (200, 200, 200)

# Game constants
ROWS = 9
COLS = 6
CELL_SIZE = 60
WIDTH = COLS * CELL_SIZE
HEIGHT = ROWS * CELL_SIZE
GAMESTATE_FILE = "gamestate.txt"

class Player(Enum):
    RED = "R"
    BLUE = "B"
    
    def opposite(self):
        return Player.BLUE if self == Player.RED else Player.RED

class GameState:
    def __init__(self):
        # Initialize empty board
        self.board = np.zeros((ROWS, COLS), dtype=object)
        for r in range(ROWS):
            for c in range(COLS):
                self.board[r, c] = [0, None]  # [count, player]
        
        self.current_player = Player.RED
        self.game_over = False
        self.winner = None
        
    def clone(self):
        """Create a deep copy of the game state"""
        new_state = GameState()
        new_state.board = copy.deepcopy(self.board)
        new_state.current_player = self.current_player
        new_state.game_over = self.game_over
        new_state.winner = self.winner
        return new_state
    
    def get_critical_mass(self, row, col):
        """Get the critical mass of a cell based on its position"""
        if (row == 0 or row == ROWS - 1) and (col == 0 or col == COLS - 1):
            return 2  # Corner
        elif row == 0 or row == ROWS - 1 or col == 0 or col == COLS - 1:
            return 3  # Edge
        else:
            return 4  # Middle
    
    def is_valid_move(self, row, col, player):
        """Check if placing an orb at (row, col) is a valid move for player"""
        if row < 0 or row >= ROWS or col < 0 or col >= COLS:
            return False
        
        # Can only place in empty cell or cell with player's orbs
        cell = self.board[row, col]
        return cell[0] == 0 or cell[1] == player
    
    def get_valid_moves(self, player):
        """Get all valid moves for the given player"""
        valid_moves = []
        for r in range(ROWS):
            for c in range(COLS):
                if self.is_valid_move(r, c, player):
                    valid_moves.append((r, c))
        return valid_moves
    
    def make_move(self, row, col):
        """Place an orb at (row, col) for current player and handle cascading explosions"""
        if not self.is_valid_move(row, col, self.current_player):
            return False
        
        # Add orb to the cell
        cell = self.board[row, col]
        if cell[0] == 0:  # If cell is empty
            cell[0] = 1
            cell[1] = self.current_player
        else:  # If cell already has orbs
            cell[0] += 1
        
        # Check for explosions and handle them
        self._handle_explosions(row, col)
        
        # Check for game over (if one player has no orbs left)
        self._check_game_over()
        
        # Switch players if game is not over
        if not self.game_over:
            self.current_player = self.current_player.opposite()
        
        return True
    
    def _handle_explosions(self, row, col):
        """Handle cascading explosions starting from (row, col)"""
        cells_to_check = [(row, col)]
        
        while cells_to_check:
            r, c = cells_to_check.pop(0)
            cell = self.board[r, c]
            critical_mass = self.get_critical_mass(r, c)
            
            # Check if cell should explode
            if cell[0] >= critical_mass:
                player = cell[1]
                
                # Reset the current cell
                cell[0] -= critical_mass
                if cell[0] == 0:
                    cell[1] = None
                
                # Add orbs to adjacent cells
                neighbors = self._get_neighbors(r, c)
                for nr, nc in neighbors:
                    neighbor_cell = self.board[nr, nc]
                    
                    # Capture opponent's orbs
                    if neighbor_cell[0] > 0 and neighbor_cell[1] != player:
                        neighbor_cell[1] = player
                    
                    # Add orb
                    neighbor_cell[0] += 1
                    if neighbor_cell[1] is None:
                        neighbor_cell[1] = player
                    
                    # Add neighbor to check for explosion
                    cells_to_check.append((nr, nc))
    
    def _get_neighbors(self, row, col):
        """Get orthogonally adjacent cells"""
        neighbors = []
        directions = [(0, 1), (1, 0), (0, -1), (-1, 0)]  # right, down, left, up
        
        for dr, dc in directions:
            nr, nc = row + dr, col + dc
            if 0 <= nr < ROWS and 0 <= nc < COLS:
                neighbors.append((nr, nc))
        
        return neighbors
    
    def _check_game_over(self):
        """Check if the game is over (one player has no orbs left)"""
        # If no moves have been made yet, game is not over
        if not self._has_any_orbs():
            return
        
        red_exists = False
        blue_exists = False
        
        for r in range(ROWS):
            for c in range(COLS):
                cell = self.board[r, c]
                if cell[0] > 0:
                    if cell[1] == Player.RED:
                        red_exists = True
                    elif cell[1] == Player.BLUE:
                        blue_exists = True
        
        if red_exists and not blue_exists:
            self.game_over = True
            self.winner = Player.RED
        elif blue_exists and not red_exists:
            self.game_over = True
            self.winner = Player.BLUE
    
    def _has_any_orbs(self):
        """Check if any orbs exist on the board"""
        for r in range(ROWS):
            for c in range(COLS):
                if self.board[r, c][0] > 0:
                    return True
        return False
    
    def count_orbs(self, player):
        """Count the total number of orbs for a player"""
        count = 0
        for r in range(ROWS):
            for c in range(COLS):
                cell = self.board[r, c]
                if cell[1] == player:
                    count += cell[0]
        return count
    
    def to_file_format(self):
        """Convert game state to file format"""
        lines = []
        for r in range(ROWS):
            row_str = []
            for c in range(COLS):
                cell = self.board[r, c]
                if cell[0] == 0:
                    row_str.append("0")
                else:
                    row_str.append(f"{cell[0]}{cell[1].value}")
            lines.append(" ".join(row_str))
        return "\n".join(lines)
    
    @staticmethod
    def from_file_format(file_content):
        """Create a game state from file content"""
        lines = file_content.strip().split("\n")
        
        # First line is the header, skip it
        board_lines = lines[1:]
        
        state = GameState()
        
        for r, line in enumerate(board_lines):
            cells = line.split()
            for c, cell_str in enumerate(cells):
                if cell_str == "0":
                    state.board[r, c] = [0, None]
                else:
                    # Parse count and player
                    count = int(cell_str[:-1])
                    player = Player.RED if cell_str[-1] == "R" else Player.BLUE
                    state.board[r, c] = [count, player]
        
        # Determine current player from the header
        if lines[0].strip() == "Human Move:":
            state.current_player = Player.BLUE  # AI's turn after human
        else:
            state.current_player = Player.RED  # Human's turn after AI
        
        # Check if the game is over
        state._check_game_over()
        
        return state


class ChainReactionUI:
    def __init__(self):
        pygame.init()
        pygame.display.set_caption("Chain Reaction")
        self.screen = pygame.display.set_mode((WIDTH, HEIGHT))
        self.clock = pygame.time.Clock()
        self.font = pygame.font.SysFont("Arial", 20)
        self.game_state = GameState()
    
    def draw_board(self):
        self.screen.fill(WHITE)
        
        # Draw grid lines
        for r in range(ROWS + 1):
            pygame.draw.line(self.screen, BLACK, (0, r * CELL_SIZE), (WIDTH, r * CELL_SIZE))
        for c in range(COLS + 1):
            pygame.draw.line(self.screen, BLACK, (c * CELL_SIZE, 0), (c * CELL_SIZE, HEIGHT))
        
        # Draw orbs
        for r in range(ROWS):
            for c in range(COLS):
                cell = self.game_state.board[r, c]
                if cell[0] > 0:
                    color = RED if cell[1] == Player.RED else BLUE
                    self.draw_orbs(r, c, cell[0], color)
        
        # Draw current player indicator at the top
        text = f"Current Player: {'Red' if self.game_state.current_player == Player.RED else 'Blue'}"
        text_surface = self.font.render(text, True, BLACK)
        self.screen.blit(text_surface, (10, 10))
        
        pygame.display.flip()
    
    def draw_orbs(self, row, col, count, color):
        # Calculate the center of the cell
        center_x = col * CELL_SIZE + CELL_SIZE // 2
        center_y = row * CELL_SIZE + CELL_SIZE // 2
        
        # Draw orbs based on count
        if count == 1:
            pygame.draw.circle(self.screen, color, (center_x, center_y), CELL_SIZE // 4)
        elif count == 2:
            pygame.draw.circle(self.screen, color, (center_x - 10, center_y), CELL_SIZE // 5)
            pygame.draw.circle(self.screen, color, (center_x + 10, center_y), CELL_SIZE // 5)
        elif count == 3:
            pygame.draw.circle(self.screen, color, (center_x, center_y - 10), CELL_SIZE // 6)
            pygame.draw.circle(self.screen, color, (center_x - 10, center_y + 5), CELL_SIZE // 6)
            pygame.draw.circle(self.screen, color, (center_x + 10, center_y + 5), CELL_SIZE // 6)
        else:
            pygame.draw.circle(self.screen, color, (center_x - 10, center_y - 10), CELL_SIZE // 7)
            pygame.draw.circle(self.screen, color, (center_x + 10, center_y - 10), CELL_SIZE // 7)
            pygame.draw.circle(self.screen, color, (center_x - 10, center_y + 10), CELL_SIZE // 7)
            pygame.draw.circle(self.screen, color, (center_x + 10, center_y + 10), CELL_SIZE // 7)
    
    def get_cell_from_pos(self, pos):
        x, y = pos
        col = x // CELL_SIZE
        row = y // CELL_SIZE
        
        if 0 <= row < ROWS and 0 <= col < COLS:
            return row, col
        
        return None
    
    def show_game_over(self):
        """Display game over message"""
        winner = "Red" if self.game_state.winner == Player.RED else "Blue"
        font = pygame.font.SysFont("Arial", 40)
        text = font.render(f"{winner} Wins!", True, BLACK)
        rect = text.get_rect(center=(WIDTH // 2, HEIGHT // 2))
        
        # Draw semi-transparent overlay
        overlay = pygame.Surface((WIDTH, HEIGHT), pygame.SRCALPHA)
        overlay.fill((255, 255, 255, 200))
        self.screen.blit(overlay, (0, 0))
        
        # Draw text
        self.screen.blit(text, rect)
        pygame.display.flip()
        
        # Wait for user to close the window
        waiting = True
        while waiting:
            for event in pygame.event.get():
                if event.type == pygame.QUIT or event.type == pygame.KEYDOWN:
                    waiting = False
        
        pygame.quit()
        sys.exit()
    
    def update_from_file(self):
        """Read the game state from the file"""
        while True:
            try:
                if os.path.exists(GAMESTATE_FILE):
                    with open(GAMESTATE_FILE, "r") as f:
                        content = f.read()
                        
                    # Check if it's the AI's move response
                    if content.startswith("AI Move:"):
                        self.game_state = GameState.from_file_format(content)
                        return
            except Exception as e:
                print(f"Error reading file: {e}")
            
            time.sleep(0.1)
    
    def write_to_file(self):
        """Write the current game state to the file"""
        header = "Human Move:"
        content = f"{header}\n{self.game_state.to_file_format()}"
        
        with open(GAMESTATE_FILE, "w") as f:
            f.write(content)


class Heuristics:
    @staticmethod
    def orb_count_difference(state, player):
        """
        Heuristic 1: Simple difference in orb count
        Returns the difference in orb count between the player and the opponent
        """
        player_orbs = state.count_orbs(player)
        opponent_orbs = state.count_orbs(player.opposite())
        return player_orbs - opponent_orbs
    
    @staticmethod
    def weighted_orb_count(state, player):
        """
        Heuristic 2: Weighted orb count
        Orbs in corners and edges are weighted more than orbs in the center
        """
        player_score = 0
        opponent_score = 0
        
        for r in range(ROWS):
            for c in range(COLS):
                cell = state.board[r, c]
                if cell[0] == 0:
                    continue
                
                # Determine cell weight based on position
                critical_mass = state.get_critical_mass(r, c)
                weight = 1.0
                if critical_mass == 2:  # Corner
                    weight = 2.0
                elif critical_mass == 3:  # Edge
                    weight = 1.5
                
                # Add weighted score
                if cell[1] == player:
                    player_score += cell[0] * weight
                else:
                    opponent_score += cell[0] * weight
        
        return player_score - opponent_score
    
    @staticmethod
    def explosion_potential(state, player):
        """
        Heuristic 3: Explosion potential
        Evaluates how close each cell is to exploding and the chain reaction potential
        """
        player_potential = 0
        opponent_potential = 0
        
        for r in range(ROWS):
            for c in range(COLS):
                cell = state.board[r, c]
                if cell[0] == 0:
                    continue
                
                critical_mass = state.get_critical_mass(r, c)
                # Potential is higher as orbs get closer to critical mass
                potential = cell[0] / critical_mass
                
                # Bonus for being one orb away from explosion
                if cell[0] == critical_mass - 1:
                    potential *= 1.5
                
                if cell[1] == player:
                    player_potential += potential
                else:
                    opponent_potential += potential
        
        return player_potential - opponent_potential
    
    @staticmethod
    def control_territory(state, player):
        """
        Heuristic 4: Territory control
        Measures how many cells each player controls
        """
        player_cells = 0
        opponent_cells = 0
        
        for r in range(ROWS):
            for c in range(COLS):
                cell = state.board[r, c]
                if cell[0] > 0:
                    if cell[1] == player:
                        player_cells += 1
                    else:
                        opponent_cells += 1
        
        return player_cells - opponent_cells
    
    @staticmethod
    def corner_control(state, player):
        """
        Heuristic 5: Corner control
        Prioritizes control of corner cells which require less orbs to explode
        """
        corners = [(0, 0), (0, COLS-1), (ROWS-1, 0), (ROWS-1, COLS-1)]
        
        player_corners = 0
        opponent_corners = 0
        
        for r, c in corners:
            cell = state.board[r, c]
            if cell[0] > 0:
                if cell[1] == player:
                    player_corners += 1
                else:
                    opponent_corners += 1
        
        return player_corners - opponent_corners
    
    @staticmethod
    def combined_heuristic(state, player):
        """
        Combined heuristic that weights multiple factors
        """
        if state.game_over:
            if state.winner == player:
                return float('inf')
            else:
                return float('-inf')
        
        h1 = Heuristics.orb_count_difference(state, player)
        h2 = Heuristics.weighted_orb_count(state, player)
        h3 = Heuristics.explosion_potential(state, player)
        h4 = Heuristics.control_territory(state, player)
        h5 = Heuristics.corner_control(state, player)
        
        # Weight the different heuristics
        return 0.3 * h1 + 0.2 * h2 + 0.3 * h3 + 0.1 * h4 + 0.1 * h5


class MinimaxAgent:
    def __init__(self, depth=3, heuristic_func=Heuristics.combined_heuristic):
        self.depth = depth
        self.heuristic_func = heuristic_func
        self.nodes_explored = 0
    
    def get_move(self, state):
        """Get the best move for the current player using minimax with alpha-beta pruning"""
        self.nodes_explored = 0
        player = state.current_player
        best_value, best_move = self.minimax(state, self.depth, float('-inf'), float('inf'), True)
        
        print(f"Minimax explored {self.nodes_explored} nodes")
        print(f"Best move: {best_move} with value: {best_value}")
        
        return best_move
    
    def minimax(self, state, depth, alpha, beta, is_maximizing):
        """Minimax algorithm with alpha-beta pruning"""
        self.nodes_explored += 1
        
        # Terminal states
        if state.game_over:
            if state.winner == state.current_player:
                return float('inf'), None
            else:
                return float('-inf'), None
        
        # Depth limit reached
        if depth == 0:
            return self.heuristic_func(state, state.current_player), None
        
        valid_moves = state.get_valid_moves(state.current_player)
        
        # No valid moves
        if not valid_moves:
            return 0, None
        
        best_move = None
        
        if is_maximizing:
            best_value = float('-inf')
            
            for move in valid_moves:
                # Create a new state after making this move
                new_state = state.clone()
                new_state.make_move(move[0], move[1])
                
                # If the player gets another turn (due to explosions),
                # continue maximizing; otherwise, start minimizing
                next_is_maximizing = new_state.current_player == state.current_player
                
                value, _ = self.minimax(new_state, depth - 1, alpha, beta, next_is_maximizing)
                
                if value > best_value:
                    best_value = value
                    best_move = move
                
                alpha = max(alpha, best_value)
                if beta <= alpha:
                    break
            
            return best_value, best_move
        
        else:
            best_value = float('inf')
            
            for move in valid_moves:
                # Create a new state after making this move
                new_state = state.clone()
                new_state.make_move(move[0], move[1])
                
                # If the opponent gets another turn, continue minimizing;
                # otherwise, start maximizing
                next_is_maximizing = new_state.current_player == state.current_player
                
                value, _ = self.minimax(new_state, depth - 1, alpha, beta, not next_is_maximizing)
                
                if value < best_value:
                    best_value = value
                    best_move = move
                
                beta = min(beta, best_value)
                if beta <= alpha:
                    break
            
            return best_value, best_move


class GameEngine:
    def __init__(self, depth=3, heuristic=Heuristics.combined_heuristic):
        self.agent = MinimaxAgent(depth=depth, heuristic_func=heuristic)
    
    def run(self):
        """Run the game engine loop"""
        while True:
            # Wait for the human move from the file
            print("Waiting for human move...")
            try:
                if os.path.exists(GAMESTATE_FILE):
                    with open(GAMESTATE_FILE, "r") as f:
                        content = f.read()
                    
                    if content.startswith("Human Move:"):
                        # Parse the game state
                        state = GameState.from_file_format(content)
                        
                        # Check if game is over
                        if state.game_over:
                            break
                        
                        # Get AI move
                        print("Computing AI move...")
                        ai_move = self.agent.get_move(state)
                        
                        # Make the move
                        state.make_move(ai_move[0], ai_move[1])
                        
                        # Write back to file
                        ai_response = f"AI Move:\n{state.to_file_format()}"
                        with open(GAMESTATE_FILE, "w") as f:
                            f.write(ai_response)
                        
                        print("AI move completed")
                        
                        # Check if game is over after AI move
                        if state.game_over:
                            break
            except Exception as e:
                print(f"Error in game engine: {e}")
            
            time.sleep(0.5)
    
    @staticmethod
    def initialize_game():
        """Initialize a new game file if it doesn't exist"""
        if not os.path.exists(GAMESTATE_FILE):
            state = GameState()
            with open(GAMESTATE_FILE, "w") as f:
                f.write(f"AI Move:\n{state.to_file_format()}")


def run_ui():
    """Run the game UI"""
    ui = ChainReactionUI()
    running = True
    
    # Check if the game state file exists, if not create it
    if not os.path.exists(GAMESTATE_FILE):
        with open(GAMESTATE_FILE, "w") as f:
            f.write(f"AI Move:\n{ui.game_state.to_file_format()}")
    else:
        # If file exists, read it and update game state
        ui.update_from_file()
    
    while running:
        ui.draw_board()
        
        # Check if game is over
        if ui.game_state.game_over:
            ui.show_game_over()
            break
        
        # Handle events
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False
            elif event.type == pygame.MOUSEBUTTONDOWN and ui.game_state.current_player == Player.RED:
                # It's human's turn
                pos = pygame.mouse.get_pos()
                cell = ui.get_cell_from_pos(pos)
                
                if cell and ui.game_state.is_valid_move(cell[0], cell[1], Player.RED):
                    # Make the move
                    ui.game_state.make_move(cell[0], cell[1])
                    
                    # If game is over, show the result
                    if ui.game_state.game_over:
                        ui.draw_board()
                        ui.show_game_over()
                        break
                    
                    # Write to file for AI to read
                    ui.write_to_file()
                    
                    # Wait for AI's response
                    ui.update_from_file()
        
        ui.clock.tick(30)
    
    pygame.quit()


def main():
    """Main function - start either the UI or the game engine based on command line args"""
    import argparse
    
    parser = argparse.ArgumentParser(description="Chain Reaction Game")
    parser.add_argument("--mode", choices=["ui", "engine"], default="ui", help="Run mode: ui or engine")
    parser.add_argument("--depth", type=int, default=3, help="Minimax search depth")
    parser.add_argument("--heuristic", type=int, default=0, help="Heuristic function (0: combined, 1-5: individual)")
    args = parser.parse_args()
    
    # Select heuristic function
    heuristic_funcs = [
        Heuristics.combined_heuristic,
        Heuristics.orb_count_difference,
        Heuristics.weighted_orb_count,
        Heuristics.explosion_potential,
        Heuristics.control_territory,
        Heuristics.corner_control
    ]
    
    heuristic_func = heuristic_funcs[args.heuristic % len(heuristic_funcs)]
    
    if args.mode == "ui":
        run_ui()
    else:
        engine = GameEngine(depth=args.depth, heuristic=heuristic_func)
        engine.run()


if __name__ == "__main__":
    main()