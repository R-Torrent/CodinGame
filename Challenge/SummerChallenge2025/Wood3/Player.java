import java.util.*;

/*
 * Summer Challenge 2025
 * Contest
 */

/*
 * Wood 3 League
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
					in.nextInt(), // x
					in.nextInt(), // y
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

	public int getWetness() { return wetness; }

	public void setMoveAction(String moveAction) { this.moveAction = moveAction; }

	public void setCombatAction(String combatAction) { this.combatAction = combatAction; }

	public void setMessageAction(String messageAction) { this.messageAction = messageAction; }

	public int compareDistanceToTarget(Agent a, Pair target) {
		return Integer.compare(this.coord.distanceTo(target), a.coord.distanceTo(target));
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