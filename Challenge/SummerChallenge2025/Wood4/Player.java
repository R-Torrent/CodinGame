import java.util.*;
import java.io.*;
import java.math.*;

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
	private final ArrayList<Agent> agents;
	int myAgentCount;
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
			System.err.println("OOPS! Brain dead");
			player.deadBrain();
		}
		finally {
			player.in.close();
		}
	}

	private void runGame() {
		// game loop
		for (; gameTurn <= limitGameTurns; gameTurn++) {
			loadTurn();
			overmind.think();

			// Write an action using System.out.println()
			// To debug: System.err.println("Debug messages...");
			System.out.println(overmind.issueCommands());
		}
	}

	private void deadBrain() {
		// game loop
		for (; gameTurn <= limitGameTurns; gameTurn++) {
			loadTurn();
		}
	}

	private void loadTurn() {
		int agentCount = in.nextInt(); // Total number of agents still in the game
		for (int i = 0; i < agentCount; i++) {
			int agentId = in.nextInt();
			agents.get(agentId).setAgent(
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
		agents = new ArrayList<>();
		for (int i = 0; i < agentDataCount; i++) {
			int agentId = in.nextInt(); // Unique identifier for this agent
			agents.add(agentId, new Agent(
					agentId,
					in.nextInt(), // Player id of this agent
					in.nextInt(), // Number of turns between each of this agent's shots
					in.nextInt(), // Maximum manhattan distance for greatest damage output
					in.nextInt(), // Damage output within optimal conditions
					in.nextInt()  // Number of splash bombs this agent can throw this game
			));
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

}

class Brain {

	private final Player player;

	Brain(Player player) {
		this.player = player;
	}

for (int i = 0; i < myAgentCount; i++) { }



	// One line per agent: <agentId>;<action1;action2;...> actions are "MOVE x y | SHOOT id | THROW x y | HUNKER_DOWN | MESSAGE text"System.out.println("HUNKER_DOWN");

}

class Agent {

	final int agentId;
	final int player;
	final int shootCooldown;
	final int optimalRange;
	final int soakingPower;

	int x;
	int y;
	int cooldown;
	int splashBombs;
	int wetness;

	public Agent(
			int agentId,
			int player,
			int shootCooldown,
			int optimalRange,
			int soakingPower,
			int splashBombs) {
		this.agentId = agentId;
		this.player = player;
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
		this.x = x;
		this.y = y;
		this.cooldown = cooldown;
		this.splashBombs = splashBombs;
		this.wetness = wetness;
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

	private final int x;
	private final int y;
	private final Type t;

	public enum Type {

		EMPTY     (0),
		LOW_COVER  (1),
		HIGH_COVER (2);

		Type(int code) { }

	}

	public Tile(int x, int y, int type) {
		this.x = x;
		this.y = y;
		switch (type) {
			case 1: t = Type.LOW_COVER; break;
			case 2: t = Type.HIGH_COVER; break;
			case 0: default: t = Type.EMPTY; break;
		}
	}

}
