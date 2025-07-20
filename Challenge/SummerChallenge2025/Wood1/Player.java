import java.util.*;
import java.util.function.Predicate;

/*
 * Summer Challenge 2025
 * Contest
 */

/*
 * Wood 1 League
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

	private static final int limitGameTurns = 40;

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
			grid.resetAgentsPresent();
			loadTurn();
			overmind.think(grid);

			// Write an action using System.out.println()
			// To debug: System.err.println("Debug messages...");
			overmind.issueCommands().forEach(System.out::println);
		}
	}

	private void deadBrain() {
		// dead game loop
		while (true) {
			Arrays.stream(agents)
					.filter(Objects::nonNull)
					.filter(Agent::isMyPlayer)
					.map(a -> {
						StringJoiner commands = new StringJoiner(";",
								a.getAgentId() + ";", "");
						commands.add(Command.MESSAGE.formCommand(null, null, gameTurn == 1
								? "gl hf"
								: "brain dead"));
						commands.add(Command.HUNKER_DOWN.formCommand(null, null, ""));
						return commands.toString();})
					.forEach(System.out::println);
			if (++gameTurn > limitGameTurns)
				break;
			loadTurn();
		}
	}

	private void loadTurn() {
		agentCount = in.nextInt(); // Total number of agents still in the game
		boolean[] stillAlive = new boolean[agents.length];
		for (int i = 0; i < agentCount; i++) {
			final int agentId = in.nextInt();
			stillAlive[agentId - 1] = true;
			Tile tile = grid.getTiles()[in.nextInt()][in.nextInt()]; // [x, y]
			agents[agentId - 1].setAgent(
					tile,
					in.nextInt(),   // Number of turns before this agent can shoot
					in.nextInt(),   // splashBombs remaining
					in.nextInt());  // Damage (0-100) this agent has taken
			tile.setAgentPresent(agents[agentId - 1]);
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
			agents[agentId - 1] = new Agent(myId, agentId,
					in.nextInt(),  // Player id of this agent
					in.nextInt(),  // Number of turns between each of this agent's shots
					in.nextInt(),  // Maximum manhattan distance for greatest damage output
					in.nextInt(),  // Damage output within optimal conditions
					in.nextInt()); // Number of splash bombs this agent can throw this game
		}
		int width = in.nextInt(); // Width of the game map
		int height = in.nextInt(); // Height of the game map
		grid = new Grid(width, height);
		for (int y = 0; y < height; y++)
			for (int x = 0; x < width; x++)
				grid.initTile(
						in.nextInt(),  // X coordinates, 0 is left edge
						in.nextInt(),  // Y coordinates, 0 is top edge
						in.nextInt()); // 0: empty, 1: low cover, 2: high cover
		grid.determineCoverArea();
		grid.determinePathing();
		gameTurn = 1;
		overmind = new Brain(this);
	}

	public Agent[] getAgents() { return agents; }

	public Grid getGrid() {	return grid; }

	public int getGameTurn() { return gameTurn; }

}

class Brain {

	private final Player player;

	private static final double cutOffCurve = 16.0;

	Brain(Player player) {
		this.player = player;
	}

	public void think(Grid grid) {
		final List<Agent> otherAgents = Arrays.stream(player.getAgents())
				.filter(Objects::nonNull)
				.filter(Predicate.not(Agent::isMyPlayer))
				.toList();
		final List<Agent> myAgents = Arrays.stream(player.getAgents())
				.filter(Objects::nonNull)
				.filter(Agent::isMyPlayer)
				.toList();

		SplashBomb.determineAllSplashBombs(grid);

		for (Agent a : myAgents) {
			final Map<Tile, Double> splashBombLocationAppraisal = new HashMap<>(grid.getTotalTiles());

			SplashBomb.gridBombing.entrySet().stream()
					.filter(e -> e.getValue().getTotalFriendlyWater() == 0)
					.forEach(e -> {
						final int x1 = e.getKey().getCoordinates().x(), y1 = e.getKey().getCoordinates().y();
						for (int x = Math.max(x1 - 4, 0); x <= Math.min(x1 + 4, grid.getWidth() - 1); x++) {
							final int ySpan = Math.max(4 - Math.abs(x - x1), 0);
							for (int y = Math.max(y1 - ySpan, 0);
									y <= Math.min(y1 + ySpan, grid.getHeight() - 1); y++) {
								final Tile t = grid.getTiles()[x][y];
								final int d = grid.getDistances(a.getTile(), t);
								if (d < Integer.MAX_VALUE)
									splashBombLocationAppraisal.merge(t,
											e.getValue().getTotalFoeWater() * cutOffCurve / (cutOffCurve + d * d),
											Double::sum);
							}
						}});
			a.setIntendedMove(splashBombLocationAppraisal.entrySet().stream()
					.max(Map.Entry.comparingByValue())
					.map(Map.Entry::getKey)
					.map(destination -> grid.getPath(a.getTile(), destination))
					.filter(l -> l.size() > 1)
					.map(l -> l.get(1)));
		}

		for (Agent a : myAgents)
			 a.setCommands(player);
	}

	public List<String> issueCommands() {
		return Arrays.stream(player.getAgents())
				.filter(Objects::nonNull)
				.map(Agent::buildCommands)
				.filter(Objects::nonNull)
				.toList();
	}

}

// One line per agent: <agentId>;<action1;action2;...>
// actions are "MOVE x y | SHOOT id | THROW x y | HUNKER_DOWN | MESSAGE text"
enum Command {

	MOVE ("MOVE ") {
		@Override
		StringJoiner construct(StringJoiner sj, Agent agent, Tile moveTo, String string) {
			return sj.add(moveTo.getCoordinates().toString());
		} },
	SHOOT ("SHOOT ") {
		@Override
		StringJoiner construct(StringJoiner sj, Agent shootAt, Tile tile, String string) {
			return sj.add(Integer.toString(shootAt.getAgentId()));
		} },
	THROW ("THROW ") {
		@Override
		StringJoiner construct(StringJoiner sj, Agent agent, Tile throwAt, String string) {
			return sj.add(throwAt.getCoordinates().toString());
		} },
	HUNKER_DOWN ("HUNKER_DOWN") {
		@Override
		StringJoiner construct(StringJoiner sj, Agent agent, Tile tile, String string) {
			return sj;
		} },
	MESSAGE ("MESSAGE ") {
		@Override
		StringJoiner construct(StringJoiner sj, Agent agent, Tile tile, String message) {
			return sj.add(message);
		} };

	private final String command;

	Command(String command) { this.command = command; }

	String formCommand(Agent agent, Tile tile, String string) {
		return construct(new StringJoiner(" ", command, ""), agent, tile, string).toString();
	}

	abstract StringJoiner construct(StringJoiner sj, Agent agent, Tile tile, String string);
}

class Agent {

	private final int agentId;
	private final boolean myPlayer;
	private final int shootCooldown;
	private final int optimalRange;
	private final int soakingPower;

	private Tile tile;
	private int cooldown;
	private int splashBombs;
	private int wetness;

	private Optional<Tile> intendedMove;
	private Optional<Tile> intendedSplashBomb;
	private Optional<String> intendedMessage;
	private String moveAction;
	private String combatAction;
	private String messageAction;

	public Agent(
			int myId,
			int agentId,
			int playerId,
			int shootCooldown,
			int optimalRange,
			int soakingPower,
			int splashBombs) {
		this.agentId = agentId;
		myPlayer = playerId == myId;
		this.shootCooldown = shootCooldown;
		this.optimalRange = optimalRange;
		this.soakingPower = soakingPower;
		this.splashBombs = splashBombs;
	}

	public void setAgent(
			Tile tile,
			int cooldown,
			int splashBombs,
			int wetness) {
		this.tile = tile;
		this.cooldown = cooldown;
		this.splashBombs = splashBombs;
		this.wetness = wetness;

		intendedMove = Optional.empty();
		intendedSplashBomb = Optional.empty();
		intendedMessage = Optional.empty();
		moveAction = null;
		combatAction = null;
		messageAction = null;
	}

	public int getAgentId() { return agentId; }

	public Tile getTile() { return tile; }

	public int getWetness() { return wetness; }

	public void setIntendedMove(Optional<Tile> intendedMove) { this.intendedMove = intendedMove; }

	public void setIntendedSplashBomb(Optional<Tile> intendedSplashBomb) {
		this.intendedSplashBomb = intendedSplashBomb;
	}

	public void setIntendedMessage(Optional<String> intendedMessage) { this.intendedMessage = intendedMessage; }

	public boolean isMyPlayer() { return myPlayer; }

	public int[][] calculateShootDamageArea(Grid grid, Tile from) {
		final int width = grid.getWidth();
		final int height = grid.getHeight();
		int[][] damageArea = new int[width][height];

		for (int y = 0; y < height; y++)
			for (int x = 0; x < width; x++) {
				final Pair target = grid.getTiles()[x][y].getCoordinates();
				final int dist = from.getCoordinates().distanceTo(target);
				if (dist <= optimalRange)
					damageArea[x][y] = soakingPower;
				else if (dist <= 2 * optimalRange)
					damageArea[x][y] = soakingPower / 2;
				Tile.Type cover = grid.getCoverAreaMap()
						.get(target)[from.getCoordinates().x()][from.getCoordinates().y()];
				if (cover == Tile.Type.HIGH_COVER)
					damageArea[x][y] /= 4;
				else if (cover == Tile.Type.LOW_COVER)
					damageArea[x][y] /= 2;
			}

		return damageArea;
	}

	public int[][] calculateShootDamageArea(Grid grid) {
		return calculateShootDamageArea(grid, tile);
	}

	public int calculateShootDamageOnTarget(Grid grid, Tile from, Agent target) {
		return calculateShootDamageArea(grid, from)[target.tile.getCoordinates().x()][target.tile.getCoordinates().y()];
	}

	public int calculateShootDamageOnTarget(Grid grid, Agent target) {
		return calculateShootDamageArea(grid)[target.tile.getCoordinates().x()][target.tile.getCoordinates().y()];
	}

	public void setCommands(Player player) {
		intendedMove.ifPresent(t -> moveAction = Command.MOVE.formCommand(null, t, null));
		intendedSplashBomb.ifPresentOrElse(
				t -> combatAction = Command.THROW.formCommand(null, t, null),
				() -> combatAction = Command.HUNKER_DOWN.formCommand(null, null, ""));
		if (player.getGameTurn() == 1)
			messageAction = Command.MESSAGE.formCommand(null, null, "gl hf");
		else
			intendedMessage.ifPresent(s -> messageAction = Command.MESSAGE.formCommand(null, null, s));
	}

	public String buildCommands() {
		if (!myPlayer)
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
	final int totalTiles;
	private final Tile[][] tiles;
	private final Map<Pair, Tile.Type[][]> coverAreaMap;
	private final int[][] distances;
	private final Tile[][] previous;

	public Grid(int width, int height) {
		this.width = width;
		this.height = height;
		totalTiles = width * height;
		tiles = new Tile[width][height];
		coverAreaMap = new HashMap<>(totalTiles);
		distances = new int[totalTiles][totalTiles];
		previous = new Tile[totalTiles][totalTiles];
	}

	public void initTile(int x, int y, int tileType) {
		tiles[x][y] = new Tile(x, y, tileType);
	}

	public void resetAgentsPresent() {
		for (int x = 0; x < width; x++)
			for (int y = 0; y < height; y++)
				tiles[x][y].setAgentPresent(null);
	}

	public int getWidth() {	return width; }

	public int getHeight() { return height;	}

	public int getTotalTiles() { return totalTiles; }

	public Tile[][] getTiles() { return tiles; }

	public Map<Pair, Tile.Type[][]> getCoverAreaMap() { return coverAreaMap; }

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

	public void determinePathing() {
		for (int i = 0; i < totalTiles; i++)
			for (int j = 0; j < totalTiles; j++) {
				if (i == j)
					continue;
				distances[i][j] = Integer.MAX_VALUE;
			}

		for (int x = 0; x < width; x++)
			for (int y = 0; y < height; y++) {
				final Tile t = tiles[x][y];
				previous[t.getIndex()][t.getIndex()] = t;
				if (t.getType() != Tile.Type.EMPTY)
					continue;
				Tile t1;
				if (x > 0 && (t1 = tiles[x - 1][y]).getType() == Tile.Type.EMPTY) {
					t.setNeighbor(Tile.Neighbor.LEFT, t1);
					distances[t.getIndex()][t1.getIndex()] = 1;
					previous[t.getIndex()][t1.getIndex()] = t;
				}
				if (x < width - 1 && (t1 = tiles[x + 1][y]).getType() == Tile.Type.EMPTY) {
					t.setNeighbor(Tile.Neighbor.RIGHT, t1);
					distances[t.getIndex()][t1.getIndex()] = 1;
					previous[t.getIndex()][t1.getIndex()] = t;
				}
				if (y > 0 && (t1 = tiles[x][y - 1]).getType() == Tile.Type.EMPTY) {
					t.setNeighbor(Tile.Neighbor.TOP, t1);
					distances[t.getIndex()][t1.getIndex()] = 1;
					previous[t.getIndex()][t1.getIndex()] = t;
				}
				if (y < height - 1 && (t1 = tiles[x][y + 1]).getType() == Tile.Type.EMPTY) {
					t.setNeighbor(Tile.Neighbor.BOTTOM, t1);
					distances[t.getIndex()][t1.getIndex()] = 1;
					previous[t.getIndex()][t1.getIndex()] = t;
				}
			}
		for (int k = 0; k < totalTiles; k++)
			for (int i = 0; i < totalTiles; i++)
				for (int j = 0; j < totalTiles; j++) {
					if (distances[i][k] == Integer.MAX_VALUE || distances[k][j] == Integer.MAX_VALUE)
						continue;
					final int d = distances[i][k] + distances[k][j];
					if (distances[i][j] > d) {
						distances[i][j] = d;
						previous[i][j] = previous[k][j];
					}
				}
	}

	public int getDistances(Tile from, Tile to) {
		return distances[from.getIndex()][to.getIndex()];
	}

	public List<Tile> getPath(Tile from, Tile to) {
		if (previous[from.getIndex()][to.getIndex()] == null)
			return Collections.emptyList();
		List<Tile> path = new ArrayList<>();
		path.add(to);
		while (!from.equals(to)) {
			to = previous[from.getIndex()][to.getIndex()];
			path.addFirst(to);
		}

		return path;
	}

}

class Tile {

	private final Pair coordinates;
	private final Type type;
	private final int index;
	private final Tile[] neighbors;
	private Agent agentPresent;

	private static int indices = 0;

	public enum Type {

		EMPTY {
			@Override
			public Type maxCover(Type comparedWith) {
				return comparedWith;
			} },
		LOW_COVER {
			@Override
			public Type maxCover(Type comparedWith) {
				return comparedWith == HIGH_COVER ? comparedWith : this;
			} },
		HIGH_COVER {
			@Override
			public Type maxCover(Type comparedWith) {
				return this;
			} };

		public abstract Type maxCover(Type comparedWith);

	}

	public enum Neighbor {

		LEFT(0),
		RIGHT(1),
		TOP(2),
		BOTTOM(3);

		private final int index;

		Neighbor(int index) { this.index = index; }

	}

	public Tile(int x, int y, int type) {
		coordinates = new Pair(x, y);
		switch (type) {
			case 1: this.type = Type.LOW_COVER; break;
			case 2: this.type = Type.HIGH_COVER; break;
			case 0: default: this.type = Type.EMPTY; break;
		}
		index = indices++;
		neighbors = new Tile[4];
	}

	public Pair getCoordinates() { return coordinates; }

	public Type getType() { return type; }

	public int getIndex() { return index; }

	public Agent getAgentPresent() { return agentPresent; }

	public void setNeighbor(Neighbor n, Tile t) {
		neighbors[n.index] = t;
	}

	public void setAgentPresent(Agent agentPresent) { this.agentPresent = agentPresent; }

	int getDamage(int[][] damageArea) {
		return damageArea[coordinates.x()][coordinates.y()];
	}

	@Override
	public boolean equals(Object o) {
		if (o == null || getClass() != o.getClass()) return false;
		Tile tile = (Tile) o;
		return index == tile.index;
	}

	@Override
	public int hashCode() {
		return Objects.hashCode(index);
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

class SplashBomb {

	private int totalFoeWater;
	private int totalFriendlyWater;

	public static Map<Tile, SplashBomb> gridBombing;
	public static final int bombWetness = 30;

	private SplashBomb(Tile tile) {
		gridBombing.put(tile, this);
	}

	public int getTotalFoeWater() { return totalFoeWater; }

	public int getTotalFriendlyWater() { return totalFriendlyWater; }

	public static void determineAllSplashBombs(Grid grid) {
		gridBombing = new HashMap<>(grid.getTotalTiles());

		for (int x1 = 0; x1 < grid.getWidth(); x1++)
			for (int y1 = 0; y1 < grid.getHeight(); y1++) {
				SplashBomb bomb = new SplashBomb(grid.getTiles()[x1][y1]);
				for (int x = Math.max(x1 - 1, 0); x <= Math.min(x1 + 1, grid.getWidth() - 1); x++)
					for (int y = Math.max(y1 - 1, 0); y <= Math.min(y1 + 1, grid.getHeight() - 1); y++)
						Optional.ofNullable(grid.getTiles()[x][y].getAgentPresent())
								.ifPresent(a -> {
									if (a.isMyPlayer())
										bomb.totalFriendlyWater += bombWetness;
									else
										bomb.totalFoeWater += bombWetness;
								});
			}
	}

}
