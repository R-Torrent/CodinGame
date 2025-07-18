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
			overmind.think();

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
									Integer.toString(a.getAgentId()) + ";", "");
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
					grid.getTiles()[in.nextInt()][in.nextInt()].getCoord(), // [x, y]
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
						in.nextInt(), // X coordinate, 0 is left edge
						in.nextInt(), // Y coordinate, 0 is top edge
						in.nextInt()  // 0: empty, 1: low cover, 2: high cover
				);
		grid.determineCoverArea();
		gameTurn = 1;
		overmind = new Brain(this);
	}

	public int getMyId() { return myId;	}

	public Agent[] getAgents() { return agents; }

	public int getGameTurn() { return gameTurn; }

}

class Brain {

	private final Player player;

	Brain(Player player) {
		this.player = player;
	}

	public void think() {
		final List<Agent> myAgents = Arrays.stream(player.getAgents())
				.filter(Objects::nonNull)
				.filter(a -> a.getPlayerId() == player.getMyId())
				.toList();

		final List<Agent> otherAgents = Arrays.stream(player.getAgents())
				.filter(Objects::nonNull)
				.filter(a -> a.getPlayerId() != player.getMyId())
				.sorted((a1, a2) -> Integer.compare(a2.getWetness(), a1.getWetness()))
				.toList();

		for (Agent a : myAgents) {
			a.setMoveAction(null);
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
		StringJoiner construct(StringJoiner sj, Agent target, Pair coord, String text) {
			return sj.add(coord.toString());
		}; },
	SHOOT ("SHOOT ") {
		@Override
		StringJoiner construct(StringJoiner sj, Agent target, Pair coord, String text) {
			return sj.add(Integer.toString(target.getAgentId()));
		}; },
	THROW ("THROW ") {
		@Override
		StringJoiner construct(StringJoiner sj, Agent target, Pair coord, String text) {
			return sj.add(coord.toString());
		}; },
	HUNKER_DOWN ("HUNKER_DOWN") {
		@Override
		StringJoiner construct(StringJoiner sj, Agent target, Pair coord, String text) {
			return sj;
		}; },
	MESSAGE ("MESSAGE ") {
		@Override
		StringJoiner construct(StringJoiner sj, Agent target, Pair coord, String text) {
			return sj.add(text);
		}; };

	private final String command;

	Command(String command) { this.command = command; }

	String formCommand(Agent target, Pair coord, String text) {
		return construct(new StringJoiner(" ", command, ""), target, coord, text).toString();
	}

	abstract StringJoiner construct(StringJoiner sj, Agent target, Pair coord, String text);
}

class Agent {

	private final int agentId;
	private final int playerId;
	private final int shootCooldown;
	private final int optimalRange;
	private final int soakingPower;

	private Pair coord;
	private int cooldown;
	private int splashBombs;
	private int wetness;
	private String moveAction;
	private String combatAction;
	private String messageAction;
	private int[][] damageArea;

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
			Pair coord,
			int cooldown,
			int splashBombs,
			int wetness) {
		this.coord = coord;
		this.cooldown = cooldown;
		this.splashBombs = splashBombs;
		this.wetness = wetness;
	}

	public int getAgentId() { return agentId; }

	public int getPlayerId() { return playerId;	}

	public int getWetness() { return wetness; }

	public void setMoveAction(String moveAction) { this.moveAction = moveAction; }

	public void setCombatAction(String combatAction) { this.combatAction = combatAction; }

	public void setMessageAction(String messageAction) { this.messageAction = messageAction; }

	public int compareDistanceToTarget(Agent a, Pair target) {
		return Integer.compare(this.coord.distanceTo(target), a.coord.distanceTo(target));
	}

	public int[][] getDamageAre(Grid grid) {
		final int width = grid.getWidth();
		final int height = grid.getHeight();
		damageArea = new int[width][height];

		for (int y = 0; y < height; y++)
			for (int x = 0; x < width; x++) {
				Tile.Type type = grid.getTiles()[x][y].getType();
				if (type != Tile.Type.EMPTY)
					continue;
				final Pair p = grid.getTiles()[x][y].getCoord();
				final int dist = coord.distanceTo(p);
				if (dist <= optimalRange)
					damageArea[x][y] = soakingPower;
				else if (dist <= 2 * optimalRange)
					damageArea[x][y] = soakingPower / 2;
				Tile.Type cover = grid.getCoverAreaMap().get(p)[x][y];
				if (cover == Tile.Type.HIGH_COVER)
					damageArea[x][y] /= 4;
				else if (cover == Tile.Type.LOW_COVER)
					damageArea[x][y] /= 2;
			}

		return damageArea;
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
				final Pair from = tiles[x1][y1].getCoord();

				if (x1 > 0) {
					final Tile.Type cover = tiles[x1 - 1][y1].getType(); // left cover
					if (cover != Tile.Type.EMPTY)
						for (int y = 0; y < height; y++)
							for (int x = 0; x < x1 - 1; x++)
								if ((Math.abs(y - y1) <= x1 - x) && from.distanceTo(tiles[x][y].getCoord()) > 2)
									if (cover == Tile.Type.HIGH_COVER)
										coverArea[x][y] = cover;
									else if (cover == Tile.Type.LOW_COVER && coverArea[x][y] == Tile.Type.EMPTY)
										coverArea[x][y] = cover;
				}
				if (x1 < width - 1) {
					final Tile.Type cover = tiles[x1 + 1][y1].getType(); // right cover
					if (cover != Tile.Type.EMPTY)
						for (int y = 0; y < height; y++)
							for (int x = width - 1; x > x1 + 1; x--)
								if ((Math.abs(y - y1) <= x - x1) && from.distanceTo(tiles[x][y].getCoord()) > 2)
									if (cover == Tile.Type.HIGH_COVER)
										coverArea[x][y] = cover;
									else if (cover == Tile.Type.LOW_COVER && coverArea[x][y] == Tile.Type.EMPTY)
										coverArea[x][y] = cover;
				}
				if (y1 > 0) {
					final Tile.Type cover = tiles[x1][y1 - 1].getType(); // up cover
					if (cover != Tile.Type.EMPTY)
						for (int x = 0; x < width; x++)
							for (int y = 0; y < y1 - 1; y++)
								if ((Math.abs(x - x1) <= y1 - y) && from.distanceTo(tiles[x][y].getCoord()) > 2)
									if (cover == Tile.Type.HIGH_COVER)
										coverArea[x][y] = cover;
									else if (cover == Tile.Type.LOW_COVER && coverArea[x][y] == Tile.Type.EMPTY)
										coverArea[x][y] = cover;
				}
				if (y1 < height - 1) {
					final Tile.Type cover = tiles[x1][y1 + 1].getType(); // down cover
					if (cover != Tile.Type.EMPTY)
						for (int x = 0; x < width; x++)
							for (int y = height - 1; y > y1 + 1; y--)
								if ((Math.abs(x - x1) <= y - y1) && from.distanceTo(tiles[x][y].getCoord()) > 2)
									if (cover == Tile.Type.HIGH_COVER)
										coverArea[x][y] = cover;
									else if (cover == Tile.Type.LOW_COVER && coverArea[x][y] == Tile.Type.EMPTY)
										coverArea[x][y] = cover;
				}
				coverAreaMap.put(from, coverArea);
			}
	}

}

class Tile {

	private final Pair coord;
	private final Type t;

	public enum Type {

		EMPTY     (0),
		LOW_COVER  (1),
		HIGH_COVER (2);

		Type(int code) { }

	}

	public Tile(int x, int y, int type) {
		this.coord = new Pair(x, y);
		switch (type) {
			case 1: t = Type.LOW_COVER; break;
			case 2: t = Type.HIGH_COVER; break;
			case 0: default: t = Type.EMPTY; break;
		}
	}

	public Pair getCoord() { return coord; }

	public Type getType() { return t; }

}

record Pair(int x, int y) {

	@Override
	public String toString() {
		return x + " " + y;
	}

	public int distanceTo(Pair p) {
		Objects.requireNonNull(p);
		return Math.abs(x - p.x) + Math.abs(y - p.y);
	}

}