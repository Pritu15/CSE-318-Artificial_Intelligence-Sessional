import React, { useEffect, useState } from 'react';

const API = 'http://localhost:8000';

const GameBoard = () => {
  const [board, setBoard] = useState([]);
  const [turn, setTurn] = useState('Human');
  const [loading, setLoading] = useState(false);

  const fetchState = async () => {
    const res = await fetch(`${API}/state`);
    const data = await res.json();
    setBoard(data.board);
    setTurn(data.header.includes('Human') ? 'Human' : 'AI');
  };

  const handleClick = async (r, c) => {
    if (loading || turn !== 'Human') return;
    setLoading(true);
    const response = await fetch(`${API}/move/human?row=${r}&col=${c}`, { method: 'POST' });
    const result = await response.json();
    if (result.status === 'invalid') {
      alert('Invalid move!');
      setLoading(false);
      return;
    }
    await fetchState();
    setTimeout(() => makeAIMove(), 1000);
  };

  const makeAIMove = async () => {
    await fetch(`${API}/move/ai`, { method: 'POST' });
    await fetchState();
    setLoading(false);
  };

  useEffect(() => {
    fetchState();
  }, []);

  return (
    <div>
      <div className="board">
        {board.map((row, rIdx) =>
          row.map((cell, cIdx) => (
            <div
              key={`${rIdx}-${cIdx}`}
              className={`cell ${cell.player}`}
              onClick={() => handleClick(rIdx, cIdx)}
            >
              {cell.orbs > 0 ? `${cell.orbs}` : ''}
            </div>
          ))
        )}
      </div>
      <div className="status">Turn: {turn}</div>
    </div>
  );
};

export default GameBoard;