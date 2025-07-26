import java.util.*;
import java.util.function.Predicate;

/*
 * Summer Challenge 2025
 * Contest
 */

/*
 * Silver League
 */

/**
 * Win the water fight by controlling the most territory, or out-soak your opponent!
 **/
class Player {

	private final Scanner in;
	private final Agent[] agents;
	private List<Agent> myAgents;
	private List<Agent> otherAgents;
	private final Grid grid;
	private final Brain overmind;
	private int gameTurn;

	private static final int limitGameTurns = 100;
	private static final boolean debug = false; // Debug messages for the following set of agents
	private static final Set<Integer> agentsToFollow = Set.of(1, 2, 3, 4, 5, 6, 7, 8, 9, 10);

	public static void main(String[] args) {
		Player player = new Player();
		try {
			player.runGame();
		} catch (Exception e) {
			e.printStackTrace(System.err);
			if (debug)
				System.err.println("OOPS! Brain is dead");
			player.deadBrain();
		}
		finally {
			player.in.close();
		}
	}

	private Player() {
		in = new Scanner(System.in);
		final int myId = in.nextInt(); // Your player id (0 or 1)
		final int agentDataCount = in.nextInt(); // Total number of agents in the game
		agents = new Agent[agentDataCount];
		for (int i = 0; i < agentDataCount; i++) {
			final int agentId = in.nextInt(); // Unique identifier for this agent
			agents[agentId - 1] = new Agent(myId, agentId,
					in.nextInt(),  // Player id of this agent
					in.nextInt(),  // Number of turns between each of this agent's shots
					in.nextInt(),  // Maximum Manhattan distance for greatest damage output
					in.nextInt(),  // Damage output within optimal conditions
					in.nextInt()); // Number of splash bombs this agent can throw this game
		}
		int width = in.nextInt();  // Width of the game map
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
		overmind = new Brain(this, grid);
		gameTurn = 1;
	}

	private void runGame() {
		// game loop
		while (gameTurn <= limitGameTurns) {
			grid.resetAgentsRemaining();

			loadTurn();

			overmind.think();

			if (debug) {
				if (gameTurn <= Brain.initialDeployment)
					System.err.println("** Initial Deployment **");
				myAgents.stream()
						.filter(a -> agentsToFollow.contains(a.getAgentId()))
						.forEach(System.err::println);
			}

			overmind.issueCommands().forEach(System.out::println);

			gameTurn++;
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
		int agentCount = in.nextInt(); // Total number of agents still in the game
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
		myAgents = Arrays.stream(agents)
				.filter(Objects::nonNull)
				.filter(Agent::isMyPlayer)
				.toList();
		if (myAgents.size() != in.nextInt()) // Number of alive agents controlled by you
			throw new RuntimeException("Something is wrong with my agent count");
		otherAgents = Arrays.stream(agents)
				.filter(Objects::nonNull)
				.filter(Predicate.not(Agent::isMyPlayer))
				.toList();
	}

	public Agent[] getAgents() { return agents; }

	public List<Agent> getMyAgents() { return myAgents; }

	public List<Agent> getOtherAgents() { return otherAgents; }

	public int getGameTurn() { return gameTurn; }

}

class Brain {

	private final Player player;
	private final Grid grid;
	private final Map<Integer, Map<Tile, Integer>> splashBombLocationAppraisal;
	private Set<Agent> otherAgents;

	// Initial deployment turns
	public static final int initialDeployment = 3;
	// Aversion to danger (AKA constant of proportionality with the gradient of the totalFoeShootingPotential)
	private static final double k0 = 15.0;
	// Constant of attraction to foes, per difference in wetness
	private static final double k1 = 25.0;
	// Threshold of wetness difference where foes become attractive
	private static final int indifferenceToWetness = 20;
	// Inverse Power by which attraction to foes falls
	private static final double invFoes = 0.75;
	// Constant of repulsion between friends
	private static final double k2 = 100.0;
	// Inverse Power by which repulsion between friends falls
	private static final double invFriends = 2.5;
	// Constant of appeal to SplashBomb runs
	private static final double splashBombAppealMultiplier = 2.5;
	// Minimum force that can induce a movement
	private static final double minForce = 15.0;

	final private static String[] messages = new String[] {
			"For 42 Barcelona!",
			"All enemies of rtorrent will die!",
			"Once more unto the breach, dear friends",
			"You rebel scum!",
			"How do you like them apples?" };
	private static final boolean[] messagesDisplayed = new boolean[messages.length];

	public Brain(final Player player, final Grid grid) {
		this.player = player;
		this.grid = grid;
		splashBombLocationAppraisal = new HashMap<>(player.getAgents().length);
	}

	public void think() {
		// Enemy shooting potential
		final int[][] totalFoeShootingPotential = new int[grid.getWidth()][grid.getHeight()];
		for (Agent foe : player.getOtherAgents()) {
			final int[][] shootDamageArea = foe.calculateShootDamagePotential(grid, foe.getLocation());
			for (int x = 0; x < grid.getWidth(); x++)
				for (int y = 0; y < grid.getHeight(); y++) {
					int shootDamage = shootDamageArea[x][y];
					if (foe.getShootCooldown() != 0 && foe.getCooldown() != 0)
						shootDamage = shootDamage
								* (foe.getShootCooldown() - foe.getCooldown()) / foe.getShootCooldown();
					totalFoeShootingPotential[x][y] += shootDamage;
				}
		}

		// SplashBomb evaluations
		for (Agent a : player.getMyAgents())
			splashBombLocationAppraisal.put(a.getAgentId() - 1, new HashMap<>(5));
		SplashBomb.determineAllSplashBombs(grid);
		SplashBomb.gridBombing.entrySet().stream()
				.filter(e -> e.getValue().getTotalFriendlyWater() == 0)
				.forEach(e -> {
					final int x1 = e.getKey().getCoordinates().x(), y1 = e.getKey().getCoordinates().y();
					for (int x = Math.max(x1 - 4, 0); x <= Math.min(x1 + 4, grid.getWidth() - 1); x++) {
						final int ySpan = Math.max(4 - Math.abs(x - x1), 0);
						for (int y = Math.max(y1 - ySpan, 0); y <= Math.min(y1 + ySpan, grid.getHeight() - 1); y++) {
							final Tile t = grid.getTiles()[x][y];
							for (Agent a : player.getMyAgents())
								if (a.getSplashBombs() > 0 && grid.getDistances(a.getLocation(), t) <= 1)
									splashBombLocationAppraisal.get(a.getAgentId() - 1).merge(t,
											e.getValue().getTotalFoeWater(),
											Integer::sum);
						}
					}
				});

		// Acting forces
		for (Agent a : player.getMyAgents()) {
			final Tile location = a.getLocation();
			final Coordinates coordinates = location.getCoordinates();
			final Vector2D xa = a.getVector();

			// Gradient to the totalFoeShootingPotential
			// Arrays of the three values centered around the agent's location [0]:{i-1} [1]:{i} [2]:{i+1}
			// -1: Indicates absence of a value (== border or obstacle)
			final int[] valuesX = extractValues(0, totalFoeShootingPotential, coordinates);
			final int[] valuesY = extractValues(1, totalFoeShootingPotential, coordinates);

			final Vector2D gradient = Vector2D.multiplyByConst(
					new Vector2D(calculateFiniteDifferences(valuesX), calculateFiniteDifferences(valuesY)), -k0);

			// Attraction to foes; set up to the optimal range
			final Vector2D foes = player.getGameTurn() > initialDeployment
					? attractionToFoes(player.getOtherAgents(), xa, location, a.getOptimalRange(), a.getWetness())
					: player.getOtherAgents().stream()
							.min(((f1, f2) ->
									grid.getDistances(location, f1.getLocation())
											- grid.getDistances(location, f2.getLocation())))
							.map(List::of)
							.map(l -> attractionToFoes(l, xa, location, 0, 0))
							.orElse(new Vector2D());

			// Repulsion from close friends
			Vector2D friends = new Vector2D();
			if (player.getGameTurn() > initialDeployment)
				for (Agent f : player.getMyAgents()) {
					if (f.equals(a))
						continue;
					Vector2D xf = f.getVector();
					Vector2D xfa = Vector2D.minus(xf, xa);
					friends = Vector2D.plus(friends, Vector2D.multiplyByConst(xfa, -k2
							* Math.pow(Vector2D.distance(xf, xa), -1 - invFriends)));
				}

			// SplashBomb appeal
			Vector2D splashBombAppeal = splashBombLocationAppraisal.get(a.getAgentId() - 1).entrySet().stream()
					.map(e -> new Coordinates(
							e.getKey().getCoordinates().x() - coordinates.x(),
							e.getKey().getCoordinates().y() - coordinates.y()).rescale(e.getValue()))
					.map(Coordinates::convertToVector)
					.reduce(new Vector2D(), Vector2D::plus);
			final int maxSplash = splashBombLocationAppraisal.get(a.getAgentId() - 1).values().stream()
					.mapToInt(i -> i)
					.max().orElse(0);
			final int sumSplashes =splashBombLocationAppraisal.get(a.getAgentId() - 1).values().stream()
					.mapToInt(i -> i)
					.sum();
			if (sumSplashes > 0)
				splashBombAppeal = Vector2D.multiplyByConst(splashBombAppeal,
						splashBombAppealMultiplier * maxSplash / sumSplashes);

			a.getForcesActingOn()[Force.DANGER_GRADIENT.type] = gradient;
			a.getForcesActingOn()[Force.INTERACTION_FOES.type] = foes;
			a.getForcesActingOn()[Force.INTERACTION_FRIENDS.type] = friends;
			a.getForcesActingOn()[Force.SPLASH_BOMB_APPEAL.type] = splashBombAppeal;

			// Attempt to move
			final Vector2D totalForce = Arrays.stream(a.getForcesActingOn()).reduce(new Vector2D(), Vector2D::plus);
			a.setTotalForce(totalForce);
			a.setIntendedPath(Optional.ofNullable(moveFrom(totalForce, coordinates))
					.map(destination -> grid.getPath(location, destination)));
		}

		// Target selection
		grid.resetAgentsRemaining(player.getMyAgents());
		SplashBomb.determineAllSplashBombs(grid);
		final List<Agent> myAgents = player.getMyAgents().stream()
				.sorted(Comparator.comparing(Agent::getOptimalRange)) // Close-range agents go first
				.toList();
		otherAgents = new HashSet<>(player.getOtherAgents());
		for (Agent a : myAgents) {
			final Tile presumedLocation = a.getIntendedMove().orElse(a.getLocation());

			final int[][] shootDamageArea = a.calculateShootDamagePotential(grid, presumedLocation);
			final Map<Agent, Integer> shootingDamage = new HashMap<>(otherAgents.size());
			if (a.getCooldown() == 0)
				otherAgents.forEach(foe ->
						shootingDamage.put(foe,	shootDamageArea[foe.getLocation().getCoordinates().x()]
								[foe.getLocation().getCoordinates().y()]));
			final Optional<Map.Entry<Agent, Integer>> intendedShootingTarget = shootingDamage.entrySet().stream()
					.filter(e -> e.getValue() > 0)
					.max((e1, e2) -> {
						final Agent a1 = e1.getKey(), a2 = e2.getKey();

						// Shoot to kill first...
						if (a1.getWetness() + e1.getValue() < Agent.deathWetness
								&& a2.getWetness() + e2.getValue() >= Agent.deathWetness)
							return -1;
						if (a1.getWetness() + e1.getValue() >= Agent.deathWetness
								&& a2.getWetness() + e2.getValue() < Agent.deathWetness)
							return 1;
						if (a1.getWetness() + e1.getValue() >= Agent.deathWetness
								&& a2.getWetness() + e2.getValue() >= Agent.deathWetness)
							return ((a2.getWetness() + e2.getValue()) - (a1.getWetness() + e1.getValue()));

						// ...then very wet targets, should the expected damage be equal...
						if (e1.getValue().equals(e2.getValue()))
							return a1.getWetness() - a2.getWetness();

						// ...but most shots are for maximizing damage
						return e1.getValue() - e2.getValue(); });

			final Map<Tile, Integer> splashBombTotalDamage = new HashMap<>(25);
			if (a.getSplashBombs() > 0) {
				final int x1 = presumedLocation.getCoordinates().x();
				final int y1 = presumedLocation.getCoordinates().y();
				for (int x = Math.max(x1 - 4, 0); x <= Math.min(x1 + 4, grid.getWidth() - 1); x++) {
					final int ySpan = Math.max(4 - Math.abs(x - x1), 0);
					for (int y = Math.max(y1 - ySpan, 0); y <= Math.min(y1 + ySpan, grid.getHeight() - 1); y++) {
						final Tile t = grid.getTiles()[x][y];
						if (SplashBomb.gridBombing.get(t).getTotalFriendlyWater() == 0)
							splashBombTotalDamage.put(t, SplashBomb.gridBombing.get(t).getTotalFoeWater());
					}
				}
			}
			final Optional<Map.Entry<Tile, Integer>> intendedSplashBomb = splashBombTotalDamage.entrySet().stream()
					.filter(e -> e.getValue() > 0)
					.max((e1, e2) -> {
						// Prioritize damage...
						if (!e1.getValue().equals(e2.getValue()))
							return e1.getValue() - e2.getValue();
						// ...but direct hits are preferable
						return SplashBomb.gridBombing.get(e1.getKey()).landsOnHead()
								- SplashBomb.gridBombing.get(e2.getKey()).landsOnHead();
					});

			intendedShootingTarget.ifPresent(e -> {
				if (intendedSplashBomb.isEmpty() || intendedSplashBomb.get().getValue() <= e.getValue()) {
					a.setIntendedShootingTarget(Optional.of(e.getKey()));
					presumedWetnessHit(e.getKey(), e.getValue());
					switch ((a.getAgentId() + player.getGameTurn()) % 40) {
						case 10: displayMessage(a, 0); break;
						case 20: displayMessage(a, 1); break;
						case 30: displayMessage(a, 2); break;
						default:
					}
				}
			});
			intendedSplashBomb.ifPresent(e -> {
				if (intendedShootingTarget.isEmpty() || intendedShootingTarget.get().getValue() < e.getValue()) {
					a.setIntendedSplashBomb(Optional.of(e.getKey()));
					SplashBomb.gridBombing.get(e.getKey()).getAgentsHit()
							.forEach(victim -> presumedWetnessHit(victim, SplashBomb.bombWetness));
					switch ((a.getAgentId() + player.getGameTurn()) % 20) {
						case  7: displayMessage(a, 3); break;
						case 17: displayMessage(a, 4); break;
						default:
					}
				}
			});
		}
	}

	private Vector2D attractionToFoes(final List<Agent> listFoes, final Vector2D xa, final Tile location,
									  final int optimalRange, final int wetness) {
		Vector2D combinedAttraction = new Vector2D();
		for (Agent f : listFoes) {
			final Vector2D xf = f.getVector();
			final Vector2D xfa = Vector2D.minus(xf, xa);
			final double x = Vector2D.distance(xf, xa);
			final double attractionForce = k1 * (Math.max(f.getWetness() - wetness, indifferenceToWetness))
					* (x - optimalRange + 0.5) * Math.pow(x, -1 -invFoes);
			final List<Tile> attractionPath = grid.getPath(location, f.getLocation());
			Vector2D attractionVector = new Vector2D();
			if (attractionForce <= 0 || attractionPath.size() <= 1)
				attractionVector = Vector2D.multiplyByConst(xfa, attractionForce / x);
			else
				for (Tile.Neighbor n : Tile.Neighbor.values())
					if (Optional.ofNullable(location.getNeighbors()[n.which]).orElse(new Tile())
							.equals(attractionPath.get(1))) {
						attractionVector =
								Vector2D.multiplyByConst(n.moveTo.convertToVector(), attractionForce);
						break;
					}
			combinedAttraction = Vector2D.plus(combinedAttraction, attractionVector);
		}

		return combinedAttraction;
	}

	private void presumedWetnessHit(final Agent presumedVictim, final int damage) {
		// Account for possible 'HUNKER_DOWN' from the victim;
		final int presumedWetness = presumedVictim.getWetness() + damage * 3 / 4;
		presumedVictim.setWetness(presumedWetness);
		if (presumedWetness >= Agent.deathWetness) {
			otherAgents.remove(presumedVictim);
			presumedVictim.getLocation().setAgentPresent(null);
			SplashBomb.determineAllSplashBombs(grid);
		}
	}

	private int[] extractValues(final int xy, final int[][] totalFoeShootingPotential, final Coordinates coordinates) {
		final int x = coordinates.x(), y = coordinates.y();
		final int[] array = new int[3];

		if (xy == 0) { // X coordinate
			array[0] = (x > 0 && grid.getTiles()[x - 1][y].getType() == Tile.Type.EMPTY)
					? totalFoeShootingPotential[x - 1][y]
					: -1;
			array[2] = (x < grid.getWidth() - 1 && grid.getTiles()[x + 1][y].getType() == Tile.Type.EMPTY)
					? totalFoeShootingPotential[x + 1][y]
					: -1;
		} else {       // Y coordinate
			array[0] = (y > 0 && grid.getTiles()[x][y - 1].getType() == Tile.Type.EMPTY)
					? totalFoeShootingPotential[x][y - 1]
					: -1;
			array[2] = (y < grid.getHeight() - 1 && grid.getTiles()[x][y + 1].getType() == Tile.Type.EMPTY)
					? totalFoeShootingPotential[x][y + 1]
					: -1;
		}
		array[1] = totalFoeShootingPotential[x][y];

		return array;
	}

	private double calculateFiniteDifferences(final int[] array) {
		if (array[0] != -1 && array[2] != -1)
			return (array[2] - array[0]) / 2.0; // Three-point central difference formula
		if (array[0] != -1)
			return array[1] - array[0];         // Backward difference
		if (array[2] != -1)
			return array[2] - array[1];         // Forward difference
		return 0.0;
	}

	private Tile moveFrom(final Vector2D totalForce, final Coordinates from) {
		final double absX = Math.abs(totalForce.x()), absY = Math.abs(totalForce.y());
		Tile destinationX = null, destinationY = null, t;

		if ((totalForce.x() >= minForce && from.x() < grid.getWidth() - 1
					&& (t = grid.getTiles()[from.x() + 1][from.y()]).getType() == Tile.Type.EMPTY
					&& Optional.ofNullable(t.getAgentPresent()).filter(Predicate.not(Agent::isMyPlayer)).isEmpty())
				|| (totalForce.x() <= -minForce && from.x() > 0
					&& (t = grid.getTiles()[from.x() - 1][from.y()]).getType() == Tile.Type.EMPTY
					&& Optional.ofNullable(t.getAgentPresent()).filter(Predicate.not(Agent::isMyPlayer)).isEmpty()))
			destinationX = t;
		if ((totalForce.y() >= minForce && from.y() < grid.getHeight() - 1
					&& (t = grid.getTiles()[from.x()][from.y() + 1]).getType() == Tile.Type.EMPTY
					&& Optional.ofNullable(t.getAgentPresent()).filter(Predicate.not(Agent::isMyPlayer)).isEmpty())
				|| (totalForce.y() <= -minForce && from.y() > 0
					&& (t = grid.getTiles()[from.x()][from.y() - 1]).getType() == Tile.Type.EMPTY)
					&& Optional.ofNullable(t.getAgentPresent()).filter(Predicate.not(Agent::isMyPlayer)).isEmpty())
			destinationY = t;
		if (absX >= absY)
			t = destinationX != null ? destinationX : destinationY;
		else
			t = destinationY != null ? destinationY : destinationX;

		return t;
	}

	private void displayMessage(final Agent a, final int n) {
		if (0 > n || n >= messagesDisplayed.length)
			throw new RuntimeException("Trying to display an erroneous message");
		if (!messagesDisplayed[n]) {
			a.setIntendedMessage(Optional.of(messages[n]));
			messagesDisplayed[n] = true;
		}
	}

	public List<String> issueCommands() {
		return Arrays.stream(player.getAgents())
				.filter(Objects::nonNull)
				.peek(a -> a.setCommands(player))
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
		StringJoiner construct(final StringJoiner sj, final Agent agent, final Tile moveTo, final String string) {
			return sj.add(moveTo.toString());
		} },
	SHOOT ("SHOOT ") {
		@Override
		StringJoiner construct(final StringJoiner sj, final Agent shootAt, final Tile tile, final String string) {
			return sj.add(Integer.toString(shootAt.getAgentId()));
		} },
	THROW ("THROW ") {
		@Override
		StringJoiner construct(final StringJoiner sj, final Agent agent, final Tile throwAt, final String string) {
			return sj.add(throwAt.toString());
		} },
	HUNKER_DOWN ("HUNKER_DOWN") {
		@Override
		StringJoiner construct(final StringJoiner sj, final Agent agent, final Tile tile, final String string) {
			return sj;
		} },
	MESSAGE ("MESSAGE ") {
		@Override
		StringJoiner construct(final StringJoiner sj, final Agent agent, final Tile tile, final String message) {
			return sj.add(message);
		} };

	private final String command;

	Command(final String command) { this.command = command; }

	String formCommand(final Agent agent, final Tile tile, final String string) {
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
	private final int initSplashBombs;

	private Tile location;
	private int cooldown;
	private int splashBombs;
	private int wetness;

	private Vector2D vector;
	private final Vector2D[] forcesActingOn;
	private Vector2D totalForces;

	private Optional<List<Tile>> intendedPath;
	private Optional<Tile> intendedMove;
	private Optional<Agent> intendedShootingTarget;
	private Optional<Tile> intendedSplashBomb;
	private Optional<String> intendedMessage;
	private String moveAction;
	private String combatAction;
	private String messageAction;

	public static final int deathWetness = 100;

	public Agent(
			final int myId,
			final int agentId,
			final int playerId,
			final int shootCooldown,
			final int optimalRange,
			final int soakingPower,
			final int initSplashBombs) {
		this.agentId = agentId;
		myPlayer = playerId == myId;
		this.shootCooldown = shootCooldown;
		this.optimalRange = optimalRange;
		this.soakingPower = soakingPower;
		this.initSplashBombs = initSplashBombs;

		forcesActingOn = new Vector2D[Force.values().length];
	}

	public void setAgent(
			final Tile tile,
			final int cooldown,
			final int splashBombs,
			final int wetness) {
		this.location = tile;
		this.cooldown = cooldown;
		this.splashBombs = splashBombs;
		this.wetness = wetness;

		vector = location.getCoordinates().convertToVector();

		intendedPath = Optional.empty();
		intendedMove = Optional.empty();
		intendedShootingTarget = Optional.empty();
		intendedSplashBomb = Optional.empty();
		intendedMessage = Optional.empty();
		moveAction = null;
		combatAction = null;
		messageAction = null;
	}

	public int getAgentId() { return agentId; }

	public boolean isMyPlayer() { return myPlayer; }

	public int getShootCooldown() { return shootCooldown; }

	public int getOptimalRange() { return optimalRange; }

	public Tile getLocation() { return location; }

	public int getCooldown() { return cooldown; }

	public int getSplashBombs() { return splashBombs; }

	public int getWetness() { return wetness; }

	public Vector2D getVector() { return vector; }

	public Vector2D[] getForcesActingOn() { return forcesActingOn; }

	public Optional<Tile> getIntendedMove() { return intendedMove; }

	public void setWetness(int wetness) { this.wetness = wetness; }

	public void setTotalForce(Vector2D totalForces) {
		this.totalForces = totalForces;
	}

	public void setIntendedPath(final Optional<List<Tile>> intendedPath) {
		this.intendedPath = intendedPath;
		intendedMove = intendedPath
				.filter(l -> l.size() > 1)
				.map(l -> l.get(1));
	}

	public void setIntendedShootingTarget(Optional<Agent> intendedShootingTarget) {
		this.intendedShootingTarget = intendedShootingTarget;
	}

	public void setIntendedSplashBomb(final Optional<Tile> intendedSplashBomb) {
		this.intendedSplashBomb = intendedSplashBomb;
	}

	public void setIntendedMessage(final Optional<String> intendedMessage) {
		this.intendedMessage = intendedMessage;
	}

	public int[][] calculateShootDamagePotential(final Grid grid, final Tile from) {
		final int width = grid.getWidth();
		final int height = grid.getHeight();
		final Coordinates shooterCoordinates = from.getCoordinates();
		final int[][] damageArea = new int[width][height];

		for (int y = 0; y < height; y++)
			for (int x = 0; x < width; x++) {
				final Coordinates target = grid.getTiles()[x][y].getCoordinates();
				final int dist = shooterCoordinates.distanceTo(target);
				if (dist <= optimalRange)
					damageArea[x][y] = soakingPower;
				else if (dist <= 2 * optimalRange)
					damageArea[x][y] = soakingPower / 2;
				final Tile.Type cover = grid.getCoverAreaMap()
						.get(target)[shooterCoordinates.x()][shooterCoordinates.y()];
				if (cover == Tile.Type.HIGH_COVER)
					damageArea[x][y] /= 4;
				else if (cover == Tile.Type.LOW_COVER)
					damageArea[x][y] /= 2;
			}

		return damageArea;
	}

	public void setCommands(final Player player) {
		intendedMove.ifPresent(t -> moveAction = Command.MOVE.formCommand(null, t, null));
		intendedShootingTarget.ifPresentOrElse(
				a -> combatAction = Command.SHOOT.formCommand(a, null, null),
				() -> intendedSplashBomb.ifPresentOrElse(
						t -> combatAction = Command.THROW.formCommand(null, t, null),
						() -> combatAction = Command.HUNKER_DOWN.formCommand(null, null, "")));
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

	@Override
	public String toString() {
		return "AgentId=" + agentId +
				"\n  myPlayer=" + myPlayer +
				"\n  shootCooldown=" + shootCooldown +
				"\n  optimalRange=" + optimalRange +
				"\n  soakingPower=" + soakingPower +
				"\n  initSplashBombs=" + initSplashBombs +
				"\n  location=" + location +
				"\n  cooldown=" + cooldown +
				"\n  splashBombs=" + splashBombs +
				"\n  wetness=" + wetness +
				"\n  forcesActingOn=" +
				"\n  " + String.format("%1$19s",Force.DANGER_GRADIENT) +
						":" + forcesActingOn[Force.DANGER_GRADIENT.type] +
				"\n  " + String.format("%1$19s",Force.INTERACTION_FRIENDS) +
						":" + forcesActingOn[Force.INTERACTION_FRIENDS.type] +
				"\n  " + String.format("%1$19s",Force.INTERACTION_FOES) +
						":" + forcesActingOn[Force.INTERACTION_FOES.type] +
				"\n  " + String.format("%1$19s",Force.SPLASH_BOMB_APPEAL) +
						":" + forcesActingOn[Force.SPLASH_BOMB_APPEAL.type] +
				"\n  totalForces=        " + totalForces +
				(intendedPath.map(tiles -> "\n  intendedPath=" + tiles).orElse("")) +
				(intendedMove.map(tile -> "\n  intendedMove=" + tile).orElse("")) +
				(intendedShootingTarget.map(agent -> "\n  intendedShootingTarget=" +
						agent.agentId).orElse("")) +
				(intendedSplashBomb.map(value -> "\n  intendedSplashBomb=" + value).orElse("")) +
				(intendedMessage.map(s -> "\n  intendedMessage=" + s).orElse(""));
	}

	@Override
	public boolean equals(Object o) {
		if (o == null || getClass() != o.getClass()) return false;
		Agent agent = (Agent) o;
		return agentId == agent.agentId;
	}

	@Override
	public int hashCode() {
		return Objects.hashCode(agentId);
	}

}

class Grid {

	private final int width;
	private final int height;
	private final int totalTiles;
	private final Tile[][] tiles;
	private final Map<Coordinates, Tile.Type[][]> coverAreaMap;
	private final int[][] distances;
	private final Tile[][] previous;

	public Grid(final int width, final int height) {
		this.width = width;
		this.height = height;
		totalTiles = width * height;
		tiles = new Tile[width][height];
		coverAreaMap = new HashMap<>(totalTiles);
		distances = new int[totalTiles][totalTiles];
		previous = new Tile[totalTiles][totalTiles];
	}

	public void initTile(final int x, final int y, final int tileType) {
		tiles[x][y] = new Tile(x, y, tileType);
	}

	public void resetAgentsRemaining() {
		for (int x = 0; x < width; x++)
			for (int y = 0; y < height; y++)
				tiles[x][y].setAgentPresent(null);
	}

	public void resetAgentsRemaining(final List<Agent> myAgents) {
		for (Agent a : myAgents)
			a.getIntendedMove().ifPresent(t -> {
				a.getLocation().setAgentPresent(null);
				t.setAgentPresent(a);
			});
	}

	public int getWidth() {	return width; }

	public int getHeight() { return height;	}

	public int getTotalTiles() { return totalTiles; }

	public Tile[][] getTiles() { return tiles; }

	public Map<Coordinates, Tile.Type[][]> getCoverAreaMap() { return coverAreaMap; }

	public void determineCoverArea() {
		for (int x1 = 0; x1 < width; x1++)
			for (int y1 = 0; y1 < height; y1++) {
				final Tile.Type[][] coverArea = new Tile.Type[width][height];
				final Coordinates from = tiles[x1][y1].getCoordinates();

				for (int x = 0; x < width; x++)
					for (int y = 0; y < height; y++)
						coverArea[x][y] = Tile.Type.EMPTY;
				if (x1 > 1) {
					final Tile.Type cover = tiles[x1 - 1][y1].getType(); // left cover
					if (cover != Tile.Type.EMPTY)
						for (int y = 0; y < height; y++)
							for (int x = 0; x < x1 - 1; x++)
								if ((x1 - x) > 2 || Math.abs(y - y1) >= 2)
									coverArea[x][y] = coverArea[x][y].maxCover(cover);
				}
				if (x1 < width - 2) {
					final Tile.Type cover = tiles[x1 + 1][y1].getType(); // right cover
					if (cover != Tile.Type.EMPTY)
						for (int y = 0; y < height; y++)
							for (int x = width - 1; x > x1 + 1; x--)
								if ((x - x1) > 2 || Math.abs(y - y1) >= 2)
									coverArea[x][y] = coverArea[x][y].maxCover(cover);
				}
				if (y1 > 1) {
					final Tile.Type cover = tiles[x1][y1 - 1].getType(); // top cover
					if (cover != Tile.Type.EMPTY)
						for (int x = 0; x < width; x++)
							for (int y = 0; y < y1 - 1; y++)
								if ((y1 - y) > 2 || Math.abs(x - x1) >= 2)
									coverArea[x][y] = coverArea[x][y].maxCover(cover);
				}
				if (y1 < height - 2) {
					final Tile.Type cover = tiles[x1][y1 + 1].getType(); // bottom cover
					if (cover != Tile.Type.EMPTY)
						for (int x = 0; x < width; x++)
							for (int y = height - 1; y > y1 + 1; y--)
								if ((y - y1) > 2 || Math.abs(x - x1) >= 2)
									coverArea[x][y] = coverArea[x][y].maxCover(cover);
				}
				coverAreaMap.put(from, coverArea);
			}
	}

	// Floyd-Warshall pathing algorithm
	// https://en.wikipedia.org/wiki/Floyd%E2%80%93Warshall_algorithm
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
		// Iteration contrary to the customary because of the way indices are constructed. With the more typical loops,
		// paths would be constructed following a "vertical first" approach, while it is in our interest to move
		// "horizontally first" when the game starts.
		for (int k = totalTiles - 1; k >= 0; k--)
			for (int i = totalTiles - 1; i >= 0; i--)
				for (int j = totalTiles - 1; j >= 0; j--) {
					if (distances[i][k] == Integer.MAX_VALUE || distances[k][j] == Integer.MAX_VALUE)
						continue;
					final int d = distances[i][k] + distances[k][j];
					if (distances[i][j] > d) {
						distances[i][j] = d;
						previous[i][j] = previous[k][j];
					}
				}
	}

	public int getDistances(final Tile from, final Tile to) {
		return distances[from.getIndex()][to.getIndex()];
	}

	public List<Tile> getPath(final Tile from, Tile to) {
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

	private final Coordinates coordinates;
	private final Type type;
	private final int index;
	private final Tile[] neighbors;
	private Agent agentPresent;

	private static int indices = 0;

	public enum Type {

		EMPTY {
			@Override
			public Type maxCover(final Type comparedWith) {
				return comparedWith;
			} },
		LOW_COVER {
			@Override
			public Type maxCover(final Type comparedWith) {
				return comparedWith == HIGH_COVER ? comparedWith : this;
			} },
		HIGH_COVER {
			@Override
			public Type maxCover(final Type comparedWith) {
				return this;
			} };

		public abstract Type maxCover(Type comparedWith);

	}

	public enum Neighbor {

		LEFT  (0, new Coordinates(-1, 0)),
		RIGHT (1, new Coordinates(+1, 0)),
		TOP   (2, new Coordinates(0, -1)),
		BOTTOM(3, new Coordinates(0, +1));

		public final int which;
		final Coordinates moveTo;

		Neighbor(final int which, Coordinates moveTo) {
			this.which = which;
			this.moveTo = moveTo;
		}

	}

	// Default "error" Tile
	public Tile() {
		coordinates = new Coordinates(-1, -1);
		type = Type.EMPTY;
		index = -1;
		neighbors = null;
	}

	public Tile(final int x, final int y, final int type) {
		coordinates = new Coordinates(x, y);
		switch (type) {
			case 1: this.type = Type.LOW_COVER; break;
			case 2: this.type = Type.HIGH_COVER; break;
			case 0: default: this.type = Type.EMPTY; break;
		}
		index = indices++;
		neighbors = new Tile[Neighbor.values().length];
	}

	public Coordinates getCoordinates() { return coordinates; }

	public Type getType() { return type; }

	public int getIndex() { return index; }

	public Tile[] getNeighbors() { return neighbors; }

	public Agent getAgentPresent() { return agentPresent; }

	public void setNeighbor(final Neighbor n, final Tile t) {
		neighbors[n.which] = t;
	}

	public void setAgentPresent(final Agent agentPresent) { this.agentPresent = agentPresent; }

	@Override
	public String toString() {
		return coordinates.toString();
	}

	@Override
	public boolean equals(final Object o) {
		if (o == null || getClass() != o.getClass()) return false;
		Tile tile = (Tile) o;
		return index == tile.index;
	}

	@Override
	public int hashCode() {
		return Objects.hashCode(index);
	}

}

record Coordinates(int x, int y) {

	@Override
	public String toString() {
		return x + " " + y;
	}

	public int distanceTo(Coordinates c) { // "Taxicab" distance
		return Math.abs(x - c.x) + Math.abs(y - c.y);
	}

	public Coordinates rescale(int factor) {
		return new Coordinates(x * factor, y * factor);
	}

	public Vector2D convertToVector() {
		return new Vector2D(x, y);
	}

}

record Vector2D(double x, double y) {

	public Vector2D() {
		this(0.0, 0.0);
	}

	@Override
	public String toString() {
		return String.format("(%1$ 8.3f,%2$ 8.3f)", x, y);
	}

	public static Vector2D plus(final Vector2D v1, final Vector2D v2) {
		return new Vector2D(v1.x + v2.x, v1.y + v2.y);
	}

	public static Vector2D minus(final Vector2D v1, final Vector2D v2) {
		return new Vector2D(v1.x - v2.x, v1.y - v2.y);
	}

	public static Vector2D multiplyByConst(final Vector2D v, double k) {
		return new Vector2D(k * v.x, k * v.y);
	}

	public static double distance(final Vector2D v1, final Vector2D v2) { // "Euclidean" distance
		return Math.sqrt((v1.x - v2.x) * (v1.x - v2.x) + (v1.y - v2.y) * (v1.y - v2.y));
	}

}

enum Force {

	DANGER_GRADIENT(0),
	INTERACTION_FRIENDS(1),
	INTERACTION_FOES(2),
	SPLASH_BOMB_APPEAL(3);

	public final int type;

	Force(final int type) { this.type = type; }

}

class SplashBomb {

	private int totalFoeWater;
	private int totalFriendlyWater;
	private int landsOnHead;
	final private Set<Agent> agentsHit;

	public static Map<Tile, SplashBomb> gridBombing;
	public static final int bombWetness = 30;

	private SplashBomb(final Tile tile) {
		gridBombing.put(tile, this);
		agentsHit = new HashSet<>(9);
	}

	public int getTotalFoeWater() { return totalFoeWater; }

	public int getTotalFriendlyWater() { return totalFriendlyWater; }

	public int landsOnHead() { return landsOnHead; }

	public Set<Agent> getAgentsHit() { return agentsHit; }

	public static void determineAllSplashBombs(final Grid grid) {
		gridBombing = new HashMap<>(grid.getTotalTiles());

		for (int x1 = 0; x1 < grid.getWidth(); x1++)
			for (int y1 = 0; y1 < grid.getHeight(); y1++) {
				final SplashBomb bomb = new SplashBomb(grid.getTiles()[x1][y1]);
				bomb.landsOnHead = grid.getTiles()[x1][y1].getAgentPresent() != null ? 1 : 0;
				for (int x = Math.max(x1 - 1, 0); x <= Math.min(x1 + 1, grid.getWidth() - 1); x++)
					for (int y = Math.max(y1 - 1, 0); y <= Math.min(y1 + 1, grid.getHeight() - 1); y++)
						Optional.ofNullable(grid.getTiles()[x][y].getAgentPresent())
								.ifPresent(a -> {
									if (a.isMyPlayer())
										bomb.totalFriendlyWater +=
												Math.min(bombWetness, Agent.deathWetness - a.getWetness());
									else
										bomb.totalFoeWater +=
												Math.min(bombWetness, Agent.deathWetness - a.getWetness());
									bomb.agentsHit.add(a);
								});
			}
	}

}
