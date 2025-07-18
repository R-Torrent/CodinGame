import java.util.*;

/*
 * Summer Challenge 2025
 * Contest
 */

/*
 * Wood 4 League
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
		for (int i = 0; i < agentCount; i++) {
			agents[in.nextInt() - 1].setAgent(
					in.nextInt(), // x
					in.nextInt(), // y
					in.nextInt(), // Number of turns before this agent can shoot
					in.nextInt(), // splashBombs remaining
					in.nextInt()  // Damage (0-100) this agent has taken
			);
		}
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
		gameTurn = 1;
		overmind = new Brain(this);
	}

	public int getMyId() { return myId;	}

	public Agent[] getAgents() { return agents; }

	public int getMyAgentCount() { return myAgentCount; }

	public int getGameTurn() { return gameTurn; }

}

class Brain {

	private final Player player;

	Brain(Player player) {
		this.player = player;
	}

	public void think() {
		for (int i = 0; i < player.getMyAgentCount(); i++) {
			// TODO: think
			;
		}
	}

	public List<String> issueCommands() {
		List<String> allCommands = new ArrayList<>();

		for (int i = 0; i < player.getAgents().length; i++) {
			final Agent a = player.getAgents()[i];
			if (a.getPlayerId() != player.getMyId())
				continue;
			final StringJoiner commands = new StringJoiner(";",
					Integer.toString(i + 1) + ";", "");
			if (player.getGameTurn() == 1)
				commands.add(Command.MESSAGE.formCommand(null,null, "gl hf"));
			// TODO: add the commands the brain has thought of here
			commands.add(Command.HUNKER_DOWN.formCommand(null, null, ""));
			allCommands.add(commands.toString());
		}

		return allCommands;
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
			int x,
			int y,
			int cooldown,
			int splashBombs,
			int wetness) {
		coord = new Pair(x, y);
		this.cooldown = cooldown;
		this.splashBombs = splashBombs;
		this.wetness = wetness;
	}

	public int getAgentId() { return agentId; }

	public int getPlayerId() { return playerId;	}

}

class Grid {

	private final int width;
	private final int height;
	private final Tile[][] tiles;

	public Grid(int width, int height) {
		this.width = width;
		this.height = height;
		tiles = new Tile[width][height];
	}

	public void initTile(int x, int y, int tileType) {
		tiles[x][y] = new Tile(x, y, tileType);
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

}

record Pair(int x, int y) {

	public Pair() { this(0,0); }

	@Override
	public String toString() {
		return x + " " + y;
	}

	public int distanceTo(Pair p) {
		Objects.requireNonNull(p);
		return Math.abs(x - p.x) + Math.abs(y - p.y);
	}

}