import java.util.*;

/*
 * Summer Challenge 2025
 * Contest
 */

/*
 * Wood 2 League
 */

/**
 * Win the water fight by controlling the most territory, or out-soak your opponent!
 **/
class Player {

	private final Scanner in;
	private final int myId;
	private final Agent[] agents;
	private int agentCount;
	private int myAgentCount;
	private final Grid grid;
	private int gameTurn;
	private final Brain overmind;

	private static final int limitGameTurns = 20;

	public static void main(String[] args) {
		Player player = new Player();
		try {
			player.runGame();
		} catch (Exception e) {
			e.printStackTrace(System.err);
			System.err.println("OOPS! Brain is dead");
			player.deadBrain();
		}
		finally {
			player.in.close();
		}
	}

	private void runGame() {
		// game loop
		for ( ; gameTurn <= limitGameTurns; gameTurn++) {
			loadTurn();
			overmind.think(grid);

			// Write an action using System.out.println()
			// To debug: System.err.println("Debug messages...");
			overmind.issueCommands().forEach(System.out::println);
		}
	}

	private void deadBrain() {
		// game loop
		for ( ; gameTurn <= limitGameTurns; gameTurn++) {
			loadTurn();
			Arrays.stream(agents)
					.filter(Objects::nonNull)
					.filter(a -> a.getPlayerId() == myId)
					.map(a -> {
						StringJoiner commands = new StringJoiner(";",
                                a.getAgentId() + ";", "");
						commands.add(Command.MESSAGE.formCommand(null, null, gameTurn == 1
								? "gl hf"
								: "brain dead"));
						commands.add(Command.HUNKER_DOWN.formCommand(null, null, ""));
						return commands.toString(); })
					.forEach(System.out::println);
		}
	}

	private void loadTurn() {
		agentCount = in.nextInt(); // Total number of agents still in the game
		boolean[] stillAlive = new boolean[agents.length];
		for (int i = 0; i < agentCount; i++) {
			final int agentId = in.nextInt();
			stillAlive[agentId - 1] = true;
			agents[agentId - 1].setAgent(
					grid.getTiles()[in.nextInt()][in.nextInt()].getCoordinates(), // [x, y]
					in.nextInt(), // Number of turns before this agent can shoot
					in.nextInt(), // splashBombs remaining
					in.nextInt()  // Damage (0-100) this agent has taken
			);
		}
		for (int i = 0; i < agents.length; i++)
			if (!stillAlive[i])
				agents[i] = null;
		myAgentCount = in.nextInt(); // Number of alive agents controlled by you
	}

	Player() {
		in = new Scanner(System.in);

		myId = in.nextInt(); // Your player id (0 or 1)
		final int agentDataCount = in.nextInt(); // Total number of agents in the game
		agents = new Agent[agentDataCount];
		for (int i = 0; i < agentDataCount; i++) {
			final int agentId = in.nextInt(); // Unique identifier for this agent
			agents[agentId - 1] = new Agent(agentId,
					in.nextInt(),  // Player id of this agent
					in.nextInt(),  // Number of turns between each of this agent's shots
					in.nextInt(),  // Maximum manhattan distance for greatest damage output
					in.nextInt(),  // Damage output within optimal conditions
					in.nextInt()); // Number of splash bombs this agent can throw this game
		}
		int width = in.nextInt(); // Width of the game map
		int height = in.nextInt(); // Height of the game map
		grid = new Grid(width, height);
		for (int i = 0; i < height; i++)
			for (int j = 0; j < width; j++)
				grid.initTile(
						in.nextInt(), // X coordinates, 0 is left edge
						in.nextInt(), // Y coordinates, 0 is top edge
						in.nextInt()  // 0: empty, 1: low cover, 2: high cover
				);
		grid.determineCoverArea();
		gameTurn = 1;
		overmind = new Brain(this);
	}

	public int getMyId() { return myId;	}

	public Agent[] getAgents() { return agents; }

	public Grid getGrid() {	return grid; }

	public int getGameTurn() { return gameTurn; }

}

class Brain {

	private final Player player;

	Brain(Player player) {
		this.player = player;
	}

	public void think(Grid grid) {
		final int[][] totalDamage = new int[grid.getWidth()][grid.getHeight()];
		final List<Agent> otherAgents = Arrays.stream(player.getAgents())
				.filter(Objects::nonNull)
				.filter(a -> a.getPlayerId() != player.getMyId())
				.toList();
		for (int y = 0; y < grid.getHeight(); y++)
			for (int x = 0; x < grid.getWidth(); x++)
				for (Agent a : otherAgents)
					totalDamage[x][y] += a.calculateDamageArea(player.getGrid())[x][y];

		final List<Agent> myAgents = Arrays.stream(player.getAgents())
				.filter(Objects::nonNull)
				.filter(a -> a.getPlayerId() == player.getMyId())
				.toList();
		for (Agent a : myAgents) {
			int x = a.getCoordinates().x(), y = a.getCoordinates().y();
			Pair destination = null;
			int bestCoverValue = Integer.MAX_VALUE, temp;
			if (x > 0) {
				Tile t = grid.getTiles()[x - 1][y];
				if (t.getType() == Tile.Type.EMPTY && (temp = t.getDamage(totalDamage)) < bestCoverValue) {
					bestCoverValue = temp;
					destination = t.getCoordinates(); // move left
				}
			}
			if (x < grid.getWidth() - 1) {
				Tile t = grid.getTiles()[x + 1][y];
				if (t.getType() == Tile.Type.EMPTY && (temp = t.getDamage(totalDamage)) < bestCoverValue) {
					bestCoverValue = temp;
					destination = t.getCoordinates(); // move right
				}
			}
			if (y > 0) {
				Tile t = grid.getTiles()[x][y - 1];
				if (t.getType() == Tile.Type.EMPTY && (temp = t.getDamage(totalDamage)) < bestCoverValue) {
					bestCoverValue = temp;
					destination = t.getCoordinates(); // move up
				}
			}
			if (y < grid.getHeight() - 1) {
				Tile t = grid.getTiles()[x][y + 1];
				if (t.getType() == Tile.Type.EMPTY && t.getDamage(totalDamage) < bestCoverValue)
					destination = t.getCoordinates(); // move down
			}

			a.setMoveAction(Command.MOVE.formCommand(null, destination, null));
			a.setCombatAction(Command.SHOOT.formCommand(otherAgents.get(0), null, null));
			a.setMessageAction( player.getGameTurn() == 1
					? Command.MESSAGE.formCommand(null, null, "gl hf")
					: null);
		}
	}

	public List<String> issueCommands() {
		return Arrays.stream(player.getAgents())
				.filter(Objects::nonNull)
				.map(a -> a.buildCommands(player))
				.filter(Objects::nonNull)
				.toList();
	}

}

// One line per agent: <agentId>;<action1;action2;...>
// actions are "MOVE x y | SHOOT id | THROW x y | HUNKER_DOWN | MESSAGE text"
enum Command {

	MOVE ("MOVE ") {
		@Override
		StringJoiner construct(StringJoiner sj, Agent target, Pair coordinates, String text) {
			return sj.add(coordinates.toString());
		} },
	SHOOT ("SHOOT ") {
		@Override
		StringJoiner construct(StringJoiner sj, Agent target, Pair coordinates, String text) {
			return sj.add(Integer.toString(target.getAgentId()));
		} },
	THROW ("THROW ") {
		@Override
		StringJoiner construct(StringJoiner sj, Agent target, Pair coordinates, String text) {
			return sj.add(coordinates.toString());
		} },
	HUNKER_DOWN ("HUNKER_DOWN") {
		@Override
		StringJoiner construct(StringJoiner sj, Agent target, Pair coordinates, String text) {
			return sj;
		} },
	MESSAGE ("MESSAGE ") {
		@Override
		StringJoiner construct(StringJoiner sj, Agent target, Pair coordinates, String text) {
			return sj.add(text);
		} };

	private final String command;

	Command(String command) { this.command = command; }

	String formCommand(Agent target, Pair coordinates, String text) {
		return construct(new StringJoiner(" ", command, ""), target, coordinates, text).toString();
	}

	abstract StringJoiner construct(StringJoiner sj, Agent target, Pair coordinates, String text);
}

class Agent {

	private final int agentId;
	private final int playerId;
	private final int shootCooldown;
	private final int optimalRange;
	private final int soakingPower;

	private Pair coordinates;
	private int cooldown;
	private int splashBombs;
	private int wetness;
	private String moveAction;
	private String combatAction;
	private String messageAction;

	public Agent(
			int agentId,
			int playerId,
			int shootCooldown,
			int optimalRange,
			int soakingPower,
			int splashBombs) {
		this.agentId = agentId;
		this.playerId = playerId;
		this.shootCooldown = shootCooldown;
		this.optimalRange = optimalRange;
		this.soakingPower = soakingPower;
		this.splashBombs = splashBombs;
	}

	public void setAgent(
			Pair coordinates,
			int cooldown,
			int splashBombs,
			int wetness) {
		this.coordinates = coordinates;
		this.cooldown = cooldown;
		this.splashBombs = splashBombs;
		this.wetness = wetness;
	}

	public int getAgentId() { return agentId; }

	public int getPlayerId() { return playerId;	}

	public Pair getCoordinates() { return coordinates; }

	public int getWetness() { return wetness; }

	public void setMoveAction(String moveAction) { this.moveAction = moveAction; }

	public void setCombatAction(String combatAction) { this.combatAction = combatAction; }

	public void setMessageAction(String messageAction) { this.messageAction = messageAction; }

	public int compareDistanceToTarget(Agent a, Pair target) {
		return Integer.compare(this.coordinates.distanceTo(target), a.coordinates.distanceTo(target));
	}

	public int[][] calculateDamageArea(Grid grid, Pair from) {
		final int width = grid.getWidth();
		final int height = grid.getHeight();
		int[][] damageArea = new int[width][height];

		for (int y = 0; y < height; y++)
			for (int x = 0; x < width; x++) {
				final Pair target = grid.getTiles()[x][y].getCoordinates();
				final int dist = from.distanceTo(target);
				if (dist <= optimalRange)
					damageArea[x][y] = soakingPower;
				else if (dist <= 2 * optimalRange)
					damageArea[x][y] = soakingPower / 2;
				Tile.Type cover = grid.getCoverAreaMap().get(target)[from.x()][from.y()];
				if (cover == Tile.Type.HIGH_COVER)
					damageArea[x][y] /= 4;
				else if (cover == Tile.Type.LOW_COVER)
					damageArea[x][y] /= 2;
			}

		return damageArea;
	}

	public int[][] calculateDamageArea(Grid grid) {
		return calculateDamageArea(grid, coordinates);
	}

	public String buildCommands(Player player) {
		if (playerId != player.getMyId())
			return null;

		final StringJoiner commands = new StringJoiner(";", agentId + ";", "");
		if (moveAction != null)
			commands.add(moveAction);
		if (combatAction != null)
			commands.add(combatAction);
		if (messageAction != null)
			commands.add(messageAction);

		return commands.toString();
	}

}

class Grid {

	private final int width;
	private final int height;
	private final Tile[][] tiles;
	private final HashMap<Pair, Tile.Type[][]> coverAreaMap;

	public Grid(int width, int height) {
		this.width = width;
		this.height = height;
		tiles = new Tile[width][height];
		coverAreaMap = new HashMap<>(width * height);
	}

	public void initTile(int x, int y, int tileType) {
		tiles[x][y] = new Tile(x, y, tileType);
	}

	public int getWidth() {	return width; }

	public int getHeight() { return height;	}

	public Tile[][] getTiles() { return tiles; }

	public HashMap<Pair, Tile.Type[][]> getCoverAreaMap() { return coverAreaMap; }

	public void determineCoverArea() {
		for (int x1 = 0; x1 < width; x1++)
			for (int y1 = 0; y1 < height; y1++) {
				final Tile.Type[][] coverArea = new Tile.Type[width][height];
				final Pair from = tiles[x1][y1].getCoordinates();

				for (int x = 0; x < width; x++)
					for (int y = 0; y < height; y++)
						coverArea[x][y] = Tile.Type.EMPTY;
				if (x1 > 1) {
					final Tile.Type cover = tiles[x1 - 1][y1].getType(); // left cover
					if (cover != Tile.Type.EMPTY)
						for (int y = 0; y < height; y++)
							for (int x = 0; x < x1 - 1; x++)
								if ((Math.abs(y - y1) <= x1 - x) && ((x1 - x) > 2 || Math.abs(y - y1) == 2))
									coverArea[x][y] = coverArea[x][y].maxCover(cover);
				}
				if (x1 < width - 2) {
					final Tile.Type cover = tiles[x1 + 1][y1].getType(); // right cover
					if (cover != Tile.Type.EMPTY)
						for (int y = 0; y < height; y++)
							for (int x = width - 1; x > x1 + 1; x--)
								if ((Math.abs(y - y1) <= x - x1) && ((x - x1) > 2 || Math.abs(y - y1) == 2))
									coverArea[x][y] = coverArea[x][y].maxCover(cover);
				}
				if (y1 > 1) {
					final Tile.Type cover = tiles[x1][y1 - 1].getType(); // top cover
					if (cover != Tile.Type.EMPTY)
						for (int x = 0; x < width; x++)
							for (int y = 0; y < y1 - 1; y++)
								if ((Math.abs(x - x1) <= y1 - y) && ((y1 - y) > 2 || Math.abs(x - x1) == 2))
									coverArea[x][y] = coverArea[x][y].maxCover(cover);
				}
				if (y1 < height - 2) {
					final Tile.Type cover = tiles[x1][y1 + 1].getType(); // bottom cover
					if (cover != Tile.Type.EMPTY)
						for (int x = 0; x < width; x++)
							for (int y = height - 1; y > y1 + 1; y--)
								if ((Math.abs(x - x1) <= y - y1) && ((y - y1) > 2 || Math.abs(x - x1) == 2))
									coverArea[x][y] = coverArea[x][y].maxCover(cover);
				}
				coverAreaMap.put(from, coverArea);
			}
	}

}

class Tile {

	private final Pair coordinates;
	private final Type type;

	public enum Type {

		EMPTY {
			@Override
			public Type maxCover(Type comparedWith) {
				return comparedWith;
			}
		},
		LOW_COVER {
			@Override
			public Type maxCover(Type comparedWith) {
				return comparedWith == HIGH_COVER ? comparedWith : this;
			}
		},
		HIGH_COVER {
			@Override
			public Type maxCover(Type comparedWith) {
				return this;
			}
		};

		public abstract Type maxCover(Type comparedWith);

	}

	public Tile(int x, int y, int type) {
		this.coordinates = new Pair(x, y);
		switch (type) {
			case 1: this.type = Type.LOW_COVER; break;
			case 2: this.type = Type.HIGH_COVER; break;
			case 0: default: this.type = Type.EMPTY; break;
		}
	}

	public Pair getCoordinates() { return coordinates; }

	public Type getType() { return type; }

	int getDamage(int[][] damageArea) {
		return damageArea[coordinates.x()][coordinates.y()];
	}

}

record Pair(int x, int y) {

	@Override
	public String toString() {
		return x + " " + y;
	}

	public int distanceTo(Pair p) { // Manhattan distance
		Objects.requireNonNull(p);
		return Math.abs(x - p.x) + Math.abs(y - p.y);
	}

}
