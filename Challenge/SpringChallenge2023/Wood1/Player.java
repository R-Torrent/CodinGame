package Challenge.SpringChallenge2023.Wood1;

import java.util.*;
import java.util.function.Predicate;
import java.util.stream.Collectors;

/*
 * Spring Challenge 2023
 * Contest
 */

/*
 * Wood 1 League
 * My swarm bee-lines for the eggs in a first stage, then forms into two columns of army ants ("marabuntas") as soon
 * as all the eggs have been collected:
 * First column (and main) forays the opposing colony's zone of influence.
 * Second column repeatedly targets the crystal source closest to base,
 *   and exhausts it before moving-on to the next gathering area.
 */

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/
class Player {

    private final Scanner in;
    private int gameTurn;
    private final AntBrain overmind;
    private Cell[] map, myBases, oppBases;
    private List<Cell> allResources;
    private int myPopulation;

    public static void main(String[] args) {
        Player player = new Player();
        try {
            player.runGame();
        }
        catch (Exception e) {
            e.printStackTrace(System.err);
            System.out.println(Command.MESSAGE.formCommand(null, null, 0, "OOPS! Brain dead"));
            player.deadBrain();
        }
        finally {
            player.in.close();
        }
    }

    Player() {
        in = new Scanner(System.in);
        gameTurn = 0;
        loadField();
        overmind = new AntBrain(this);
    }

    private void runGame() {
        overmind.musterColony();

        // game loop
        while (true) {
            loadTurn();
            overmind.think();

            // Write an action using System.out.println()
            // To debug: System.err.println("Debug messages...");
            System.out.println(overmind.issueCommands());

            gameTurn++;
        }

    }

    private void deadBrain() {
        while (true) {
            loadTurn();
            System.out.println(Command.WAIT.formCommand(null, null, 0, null));
        }
    }

    private void loadField() {
        int numberOfCells = in.nextInt(); // amount of hexagonal cells in this map
        map = new Cell[numberOfCells];

        int[][] neighbors = new int[numberOfCells][6];
        for (int i = 0; i < numberOfCells; i++) {
            int type = in.nextInt(); // 0 for empty, 1 for eggs, 2 for crystal
            int initialResources = in.nextInt(); // the initial amount of eggs/crystals on this cell
            int neigh0 = in.nextInt(); // the index of the neighbouring cell for each direction
            int neigh1 = in.nextInt();
            int neigh2 = in.nextInt();
            int neigh3 = in.nextInt();
            int neigh4 = in.nextInt();
            int neigh5 = in.nextInt();
            neighbors[i] = new int[]{ neigh0, neigh1, neigh2, neigh3, neigh4, neigh5 };
            map[i] = new Cell(i, Resource.determineResource(type), initialResources);
        }
        for (Cell c : map)
            c.setNeighbors(neighbors[c.getIndex()], map);

        int numberOfBases = in.nextInt();
        myBases = new Cell[numberOfBases];
        for (int i = 0; i < numberOfBases; i++) {
            int myBaseIndex = in.nextInt();
            myBases[i] = map[myBaseIndex];
        }
        oppBases = new Cell[numberOfBases];
        for (int i = 0; i < numberOfBases; i++) {
            int oppBaseIndex = in.nextInt();
            oppBases[i] = map[oppBaseIndex];
        }

        allResources = Arrays.stream(map).filter(Cell::hasResources).collect(Collectors.toList());
    }

    private void loadTurn() {
        myPopulation = 0;
        for (Cell c : map) {
            int resources = in.nextInt(); // the current amount of eggs/crystals on this cell
            int myAnts = in.nextInt(); // the amount of your ants on this cell
            int oppAnts = in.nextInt(); // the amount of opponent ants on this cell
            c.setQuantity(resources);
            c.setMyAnts(myAnts);
            c.setOppAnts(oppAnts);
            myPopulation += myAnts;
        }
    }

    int getGameTurn() { return gameTurn; }

    Cell[] getMap() { return map; }

    Cell[] getMyBases() { return myBases; }

    Cell[] getOppBases() { return oppBases; }

    List<Cell> getAllResources() { return allResources; }

    int getMyPopulation() { return myPopulation; }

}

class AntBrain {

    private final Player player;
    private static int[][] shortestPath;
    private final Cell base0; // To be removed as # of bases increases in higher leagues
    private final List<Cell> allResources;
    int myAnts;
    List<Marabunta> marabuntas;
    List<Cell> availableTargets;

    AntBrain(Player player) {
        this.player = player;
        shortestPath = shortestPaths(player.getMap());
        base0 = player.getMyBases()[0];
        allResources = player.getAllResources();
    }

    void musterColony() {
        marabuntas = new ArrayList<>(3);
        marabuntas.add(new Marabunta("Raiders", 0, base0, Strategy.HARVESTFAR));
        marabuntas.add(new Marabunta("LocalBoys", 0, base0, Strategy.HARVESTCLOSE));
        marabuntas.add(new Marabunta("FabergéCollectors", 0, base0, Strategy.EGGSEEKER));
    }

    void think() {
        myAnts = player.getMyPopulation();

        allResources.removeIf(c -> !c.hasResources());
        // targets order by proximity to base (first) and greater size (second)
        allResources.sort(comparatorDistancesTo(base0));

        availableTargets = new ArrayList<>(allResources);
        marabuntas.forEach(m -> m.getTargets().removeIf(c -> !availableTargets.contains(c)));
        availableTargets.removeAll(marabuntas.stream().map(Marabunta::getTargets)
                .flatMap(List::stream).collect(Collectors.toSet()));
        marabuntas.forEach(m -> {
            m.clearMemory();
            if (!m.getHead().filter(Cell::hasResources).isPresent())
                m.setHead(availableTargets);
            assignAnts(m);
            if (!m.isPopulated())
                m.clearTargets(availableTargets);
        });
        availableTargets.sort(comparatorDistancesTo(base0));

        marabuntas.stream().filter(Marabunta::isPopulated).forEach(m -> {
            for (Link l : m.constructPath(availableTargets))
                m.addCommand(Command.LINE.formCommand(l.getFrom(), l.getTo(), m.density(), null));
        });
    }

    void assignAnts(Marabunta marabunta) {
        if (allResources.stream()
                .anyMatch(((Predicate<Cell>)c -> c.getResource().isType(Resource.EGGS))
                        .and(c -> distance(base0, c) < myAnts))) {
            marabunta.setSize(marabunta.getStrategy().isStrategy(Strategy.EGGSEEKER) ? myAnts : 0);
        }
        else
            if (!marabunta.getTargets().isEmpty())
                switch (marabunta.getStrategy()) {
                    case HARVESTFAR:
                        marabunta.setSize(myAnts * 6 / 10);
                        break;
                    case HARVESTCLOSE:
                        marabunta.setSize(myAnts * 4 / 10);
                        break;
                    case EGGSEEKER:
                        marabunta.setSize(0);
                }
            else
                marabunta.setSize(0);
    }

    String issueCommands() {
        StringJoiner commands = new StringJoiner(";");

        if (player.getGameTurn() == 0)
            commands.add(Command.MESSAGE.formCommand(null, null, 0, "gl hf"));
        marabuntas.stream().map(Marabunta::getCommands).flatMap(List::stream).forEach(commands::add);

        return commands.toString();
    }

    static Optional<Cell> findClosestTo(Cell to, List<Cell> pool) {
        List<Cell> copy = new ArrayList<>(pool);
        copy.remove(to);
        copy.sort(comparatorDistancesTo(to));

        return copy.stream().findFirst();
    }

    static int distance(Cell from, Cell to) {
        return shortestPath[from.getIndex()][to.getIndex()];
    }

    static int distance(Cell from, Cell to, Cell via) {
        return distance(from, via) + distance(via, to);
    }

    static private Comparator<Cell> comparatorDistancesTo(Cell to) {
        // targets order by proximity (first) and greater size (second)
        return (c1, c2) -> {
            int path = shortestPath[to.getIndex()][c1.getIndex()] - shortestPath[to.getIndex()][c2.getIndex()];
            return path != 0 ? path : c2.getQuantity() - c1.getQuantity();
        };
    }

    // Floyd-Warshall algorithm
    static private int[][] shortestPaths(Cell[] map) {
        final int N = map.length;
        int[][] shortestPath = new int[N][N];

        for (int i = 0; i < N; i++) {
            shortestPath[i][i] = 0;
            for (int j = i + 1; j < N; j++)
                shortestPath[j][i] = shortestPath[i][j] = map[i].isNeighborWith(j) ? 1 : N;
        }
        for (int k = 0; k < N; k++)
            for (int i = 0; i < N; i++)
                for (int j = i + 1; j < N; j++) {
                    int path = shortestPath[i][k] + shortestPath[k][j];
                    if (shortestPath[i][j] > path)
                        shortestPath[j][i] = shortestPath[i][j] = path;
                }

        return shortestPath;
    }

}

class Marabunta {

    final String name;
    private int size;
    final private Cell base;
    final private Strategy strategy;
    final private List<String> commands;
    private Cell head;
    final private LinkedList<Cell> targets;
    final private LinkedList<Link> path;


    Marabunta(String name, int size, Cell base, Strategy strategy)
    {
        this.name = name;
        this.size = size;
        this.base = base;
        this.strategy = strategy;
        commands = new ArrayList<>();
        targets = new LinkedList<>();
        path = new LinkedList<>();
    }

    String getName() { return name; }

    boolean isPopulated() {
        return size > 0;
    }

    void setSize(int size) { this.size = size; }

    public Strategy getStrategy() { return strategy; }

    List<String> getCommands() { return commands; }

    void addCommand(String command) {
        commands.add(command);
    }

    void clearMemory() {
        commands.clear();
    }

    Optional<Cell> getHead() {
        return Optional.ofNullable(head);
    }

    List<Cell> getTargets() { return targets; }

    void clearTargets(List<Cell> availableTargets) {
        head = null;
        targets.stream().filter(Cell::hasResources).forEach(availableTargets::add);
        targets.clear();
        System.err.println(name + ": head & targets cleared");
    }

    void setHead(List<Cell> availableTargets) {
        if (strategy.isStrategy(Strategy.EGGSEEKER))
            clearTargets(availableTargets);
        Optional<Cell> newHead = strategy.selectNextTarget(head, availableTargets);
        newHead.ifPresent(c -> {
            availableTargets.remove(c);
            targets.add(c);
            System.err.println(name + ": target added to head <- " + c.getIndex());
        });
        head = newHead.orElse(null);
    }

    public List<Link> constructPath(List<Cell> availableTargets) {
        path.clear();
        targets.forEach(c -> path.add(new Link(path.isEmpty() ? base : path.getLast().getTo(), c)));
        System.err.println(name + ": links before search");
        for (Link l : path)
            System.err.println(l.getFrom().getIndex() + " ->- " + l.getTo().getIndex());

        int limDeviation = Math.min(
                size - (path.stream().mapToInt(Link::getLength).sum() + 1),
                strategy.getDeviationFromPath());
        for (int i = 0; i < path.size(); i++) {
            Link link = path.get(i);
            List<Cell> candidates = new ArrayList<>();
            for (int deviation = 0;  deviation <= limDeviation; deviation++) {
                final int finalDeviation = deviation;
                candidates = availableTargets.stream()
                        .filter(((Predicate<Cell>)c -> (AntBrain.distance(link.getFrom(), link.getTo(), c)
                                - link.getLength() <= finalDeviation))
                                .and(c -> AntBrain.distance(base, c) <= AntBrain.distance(base, link.getTo())))
                        .collect(Collectors.toList());
                if (!candidates.isEmpty())
                    break;
            }
            if (!candidates.isEmpty()) {
                candidates.sort((c1, c2) -> c2.getQuantity() - c1.getQuantity());
                Cell candidate = candidates.get(0);
                System.err.println(name + ": target added to path <-+ " + candidate.getIndex());
                targets.add(i, candidate);
                path.set(i, new Link(candidate, link.getTo()));
                path.add(i, new Link(link.getFrom(), candidate));
                availableTargets.remove(candidate);
                break;
            }
        }
        System.err.println(name + ": links after search");
        for (Link l : path)
            System.err.println(l.getFrom().getIndex() + " ->- " + l.getTo().getIndex());

        return path;
    }

    int density() {
        return size / (path.stream().mapToInt(Link::getLength).sum() + 1);
    }

}

class Link {

    private final Cell from;
    private final Cell to;
    private final int length;

    Link(Cell from, Cell to) {
        this.from = from;
        this.to = to;
        length = AntBrain.distance(from, to);
    }

    public Cell getFrom() { return from; }

    public Cell getTo() { return to; }

    public int getLength() { return length; }

}

enum Strategy {

    HARVESTFAR (3) {
        Optional<Cell> selectNextTarget(Cell head, List<Cell> availableTargets) {
            if (availableTargets.isEmpty()) return Optional.empty();
            return Optional.ofNullable(head)
                    .map(c -> AntBrain.findClosestTo(c, availableTargets))
                    .orElse(Optional.of(availableTargets.get(availableTargets.size() / 2)));
        } },
    HARVESTCLOSE (2) {
        Optional<Cell> selectNextTarget(Cell head, List<Cell> availableTargets) {
            if (availableTargets.isEmpty()) return Optional.empty();
            return Optional.ofNullable(head)
                    .map(c -> AntBrain.findClosestTo(c, availableTargets))
                    .orElse(Optional.of(availableTargets.get(availableTargets.size() / 4)));
        } },
    EGGSEEKER(2) {
        Optional<Cell> selectNextTarget(Cell head, List<Cell> availableTargets) {
            return availableTargets.stream()
                    .filter(c -> c.getResource().isType(Resource.EGGS))
                    .findFirst();
        } };

    private final int deviationFromPath;

    Strategy(int deviationFromPath) { this.deviationFromPath = deviationFromPath; }

    boolean isStrategy(Strategy strategy) { return this.equals(strategy); }

    int getDeviationFromPath() { return deviationFromPath; }

    abstract Optional<Cell> selectNextTarget(Cell head, List<Cell> availableTargets);

}

class Cell {

    private final int index;
    private final Resource resource;
    private int quantity;
    private Cell[] neighbors;
    private int myAnts;
    private int oppAnts;

    Cell(int index, Resource resource, int quantity) {
        this.index = index;
        this.resource = resource;
        this.quantity = quantity;
    }

    int getIndex() { return index; }

    Resource getResource() { return resource; }

    int getQuantity() { return quantity; }

    boolean hasResources() {
        return quantity > 0;
    }

    void setQuantity(int quantity) { this.quantity = quantity; }

    boolean isNeighborWith(int indexN) {
        for (Cell c : neighbors)
            if (c != null && c.index == indexN)
                return true;
        return false;
    }

    void setNeighbors(int[] indices, Cell[] map) {
        neighbors = new Cell[6];
        for (int i = 0; i < 6; i++)
            neighbors[i] = indices[i] >= 0 ? map[indices[i]] : null;
    }

    int getMyAnts() { return myAnts; }

    void setMyAnts(int myAnts) { this.myAnts = myAnts; }

    int getOppAnts() { return oppAnts; }

    void setOppAnts(int oppAnts) { this.oppAnts = oppAnts; }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        Cell cell = (Cell) o;
        return index == cell.index;
    }

    @Override
    public int hashCode() {
        return Objects.hash(index);
    }

}

enum Resource {

    NONE    (0),
    EGGS    (1),
    CRYSTAL (2);

    final int type;

    Resource(int type) { this.type = type; }

    boolean isType(Resource resource) {
        return this.type == resource.type;
    }

    static Resource determineResource(int type) {
        switch (type) {
            case 1: return Resource.EGGS;
            case 2: return Resource.CRYSTAL;
            default: return Resource.NONE;
        }
    }

}

// WAIT | LINE <sourceIdx> <targetIdx> <strength> | BEACON <cellIdx> <strength> | MESSAGE <text>
enum Command {

    BEACON ("BEACON ") {
        StringJoiner construct(StringJoiner sj, Cell cell1, Cell cell2, int strength, String text) {
            return sj.add(Integer.toString(cell2.getIndex()))
                     .add(Integer.toString(strength));
        }; },
    LINE ("LINE ") {
        StringJoiner construct(StringJoiner sj, Cell cell1, Cell cell2, int strength, String text) {
            return sj.add(Integer.toString(cell1.getIndex()))
                     .add(Integer.toString(cell2.getIndex()))
                     .add(Integer.toString(strength));
        }; },
    WAIT ("WAIT") {
        StringJoiner construct(StringJoiner sj, Cell cell1, Cell cell2, int strength, String text) {
            return sj;
        }; },
    MESSAGE ("MESSAGE ") {
        StringJoiner construct(StringJoiner sj, Cell cell1, Cell cell2, int strength, String text) {
            return sj.add(text);
        }; };

    private final String command;

    Command(String command) { this.command = command; }

    String formCommand(Cell cell1, Cell cell2, int strength, String text) {
        return construct(new StringJoiner(" ", command, ""), cell1, cell2, strength, text).toString();
    }

    abstract StringJoiner construct(StringJoiner sj, Cell cell1, Cell cell2, int strength, String text);

}