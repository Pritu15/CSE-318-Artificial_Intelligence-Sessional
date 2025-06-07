# === backend/main.py ===
from fastapi import FastAPI
from utils import read_gamestate, write_gamestate
from game import apply_human_move, apply_ai_move
from fastapi.middleware.cors import CORSMiddleware

app = FastAPI()

app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

@app.post("/move/human")
def human_move(row: int, col: int):
    header, board = read_gamestate()
    new_board, valid = apply_human_move(board, row, col)
    if not valid:
        return {"status": "invalid"}
    write_gamestate("Human Move:", new_board)
    return {"status": "ok"}

@app.post("/move/ai")
def ai_move():
    header, board = read_gamestate()
    new_board = apply_ai_move(board)
    write_gamestate("AI Move:", new_board)
    return {"status": "ok"}

@app.get("/state")
def get_state():
    header, board = read_gamestate()
    return {"header": header, "board": board}