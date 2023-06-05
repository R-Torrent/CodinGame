package Challenge.SpringChallenge2023.Bronze;

import java.util.*;
import java.util.function.Predicate;
import java.util.stream.Collectors;

/*
 * Spring Challenge 2023
 * Contest
 */

/*
 * Bronze League
 * Semi-independent bases
 * Swarms form into columns of army ants ("marabuntas"). They prioritize egg-gathering until half the crystals have been
 * harvested. At that point they transition to ambivalent resource gathering. Marabuntas enter a crystal-only frenzy as
 * soon as those sources become equal or lower in number than themselves.
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
        gameTurn = 1;
        loadField();
        overmind = new AntBrain(this);
    }

    private void runGame() {
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
    private final Anthill[] anthills;
    private final List<Cell> allResources;

    @SuppressWarnings("unchecked")
    AntBrain(Player player) {
        this.player = player;

        shortestPath = shortestPaths(player.getMap());

        final int nbases = player.getMyBases().length;
        anthills = new Anthill[nbases];
        allResources = player.getAllResources();
        List<Cell>[] assignedResources = (List<Cell>[])new ArrayList<?>[nbases];
        for (int i = 0; i < nbases; i++)
            assignedResources[i] = new ArrayList<>(allResources.size());
        for (Cell c : allResources) {
            List<Link> links = Arrays.stream(player.getMyBases())
                    .map(b -> new Link(b, c))
                    .sorted((l1, l2) -> l1.getLength() - l2.getLength())
                    .collect(Collectors.toList());
            Cell appropriateBase = links.get(0).getFrom();
            for (int i = 0; i < nbases; i++)
                if (player.getMyBases()[i].equals(appropriateBase)) {
                    assignedResources[i].add(c);
                    break;
                }
        }
        for (int i = 0; i < nbases; i++)
            anthills[i] = new Anthill(player.getMyBases()[i], assignedResources[i]);
    }

    void think() {
        allResources.removeIf(c -> !c.hasResources());
        for (Anthill a : anthills)
            a.runMound(allResources, player.getGameTurn());

        int activeMounds = (int)Arrays.stream(anthills).filter(Anthill::isActive).count();
        int myAnts = player.getMyPopulation();
        for (Anthill a : anthills) {
            if (!a.isActive()) {
                System.err.println("Anthill " + a.getBase().getIndex() + " inactive");
                continue;
            }
            int antsInMound = myAnts / activeMounds;
            a.marchForth(antsInMound);
            myAnts -= antsInMound;
            activeMounds--;
        }
    }

    String issueCommands() {
        StringJoiner commands = new StringJoiner(";");

        if (player.getGameTurn() == 1)
            commands.add(Command.MESSAGE.formCommand(null, null, 0, "gl hf"));

        if (Arrays.stream(anthills).anyMatch(Anthill::isActive))
            Arrays.stream(anthills)
                    .map(Anthill::getMarabuntas)
                    .flatMap(List::stream)
                    .map(Marabunta::getCommands)
                    .flatMap(List::stream)
                    .forEach(commands::add);
        else
            for (Anthill a : anthills)
                commands.add(Command.BEACON.formCommand(null, a.getBase(), player.getMyPopulation(), null));

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

    static Comparator<Cell> comparatorDistancesTo(Cell to) {
        // targets order by proximity (first) and greater size (second)
        return (c1, c2) -> {
            int path = shortestPath[c1.getIndex()][to.getIndex()] - shortestPath[c2.getIndex()][to.getIndex()];
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

class Anthill {

    private final Cell base;
    private final List<Cell> assignedResources;
    private final int initResourcesPatches;
    private final List<Marabunta> marabuntas;
    private List<Cell> availableTargets;
    private int activeMarabuntas;

    Anthill(Cell base, List<Cell> assignedResources) {
        this.base = base;
        this.assignedResources = assignedResources;
        initResourcesPatches = assignedResources.size();

        System.err.println("Anthill " + base.getIndex() + " established");
        final int n = Math.round((float)Math.sqrt(assignedResources.size()));
        marabuntas = new ArrayList<>(n);
        for (int i = 0; i < n; i++)
            marabuntas.add(new Marabunta("mound_" + base.getIndex() + "_mara_" + i, base));
    }

    Cell getBase() { return base; }

    List<Marabunta> getMarabuntas() { return marabuntas; }

    boolean isActive() { return marabuntas.size() > 0; }

    void runMound(List<Cell> allResources, int gameTurn) {
        assignedResources.removeIf(c -> !allResources.contains(c));
        // targets order by proximity to base (first) and greater size (second)
        assignedResources.sort(AntBrain.comparatorDistancesTo(base));

        if (assignedResources.size() <= initResourcesPatches / 2 || gameTurn == 50)
            marabuntas.stream()
                    .filter(m -> m.getStrategy().isStrategy(Strategy.EGGHOARDER))
                    .forEach(m -> m.setStrategy(Strategy.GATHERER));
        if (assignedResources.stream()
                .filter(c -> c.getResource().isType(Resource.CRYSTAL)).count() <= marabuntas.size())
            marabuntas.forEach(m -> m.setStrategy(Strategy.CRYSTALOBSESSION));

        availableTargets = new ArrayList<>(assignedResources);
        marabuntas.forEach(m -> m.getTargets().removeIf(c -> !availableTargets.contains(c)));
        availableTargets.removeAll(marabuntas.stream().map(Marabunta::getTargets)
                .flatMap(List::stream).collect(Collectors.toSet()));
        marabuntas.forEach(m -> {
            m.clearMemory();
            if (!m.getHead().filter(Cell::richDeposit).isPresent())
                m.setHead(availableTargets);
            if (!m.markForAction())
                m.clearTargets(availableTargets);
            availableTargets.sort(AntBrain.comparatorDistancesTo(base));
        });

        marabuntas.stream().filter(m -> !m.isActive()).forEach(m -> System.err.println(m.getName() + ": disbanded"));
        marabuntas.removeIf(m -> !m.isActive());
        activeMarabuntas = (int)marabuntas.stream().filter(Marabunta::isActive).count();
    }

    void marchForth(int antsInMound) {
        for (Marabunta m : marabuntas) {
            int antsPerMarabunta = antsInMound / activeMarabuntas;
            m.setSize(antsPerMarabunta);
            antsInMound -= antsPerMarabunta;
            activeMarabuntas--;
        }
        marabuntas.stream().forEach(m -> {
            for (Link l : m.constructPath(availableTargets))
                m.addCommand(Command.LINE.formCommand(l.getFrom(), l.getTo(), m.density(), null));
        });
    }

}

class Marabunta {

    private final String name;
    private final Cell base;
    private int size;
    private Strategy strategy;
    private final List<String> commands;
    private boolean activeDuty;
    private Cell head;
    private final LinkedList<Cell> targets;
    private final LinkedList<Link> path;

    Marabunta(String name, Cell base)
    {
        this.name = name;
        this.base = base;
        size = 0;
        strategy = Strategy.EGGHOARDER;
        commands = new ArrayList<>();
        targets = new LinkedList<>();
        path = new LinkedList<>();
        System.err.println(name + ": marabunta called up with strategy " + strategy.name());
    }

    String getName() { return name; }

    void setSize(int size) {
        if (size != this.size) {
            this.size = size;
            System.err.println(name + ": size of troop <-+ " + size);
        }
    }

    Strategy getStrategy() { return strategy; }

    void setStrategy(Strategy strategy) {
        if (!strategy.equals(this.strategy)) {
            this.strategy = strategy;
            System.err.println(name + ": stance modified <-+ " + strategy.name());
        }
    }

    List<String> getCommands() { return commands; }

    void addCommand(String command) {
        commands.add(command);
    }

    void clearMemory() {
        commands.clear();
    }

    boolean isActive() { return activeDuty; }

    boolean markForAction() {
        return activeDuty = !targets.isEmpty();
    }

    Optional<Cell> getHead() {
        return Optional.ofNullable(head);
    }

    void setHead(List<Cell> availableTargets) {
        Optional<Cell> newHead = strategy.selectNextTarget(head, availableTargets);
        if (!newHead.isPresent() && strategy.isStrategy(Strategy.EGGHOARDER)) {
            setStrategy(Strategy.GATHERER);
            newHead = strategy.selectNextTarget(head, availableTargets);
        }
        newHead.ifPresent(c -> {
            switch (strategy) {
                case EGGHOARDER:
                    targets.stream()
                            .filter(t -> t.getResource().isType(Resource.CRYSTAL))
                            .forEach(t -> {
                                    availableTargets.add(t);
                                    System.err.println(name + ": target removed from path +-> " + t.getIndex());
                            });
                    targets.removeIf(t -> t.getResource().isType(Resource.CRYSTAL));
                    break;
                case CRYSTALOBSESSION:
                    targets.stream()
                            .filter(t -> t.getResource().isType(Resource.EGGS))
                            .forEach(t -> {
                                availableTargets.add(t);
                                System.err.println(name + ": target removed from path +-> " + t.getIndex());
                            });
                    targets.removeIf(t -> t.getResource().isType(Resource.EGGS));
                    break;
                default: break;
            }
            availableTargets.remove(c);
            targets.add(c);
            System.err.println(name + ": target added to head <-+ " + c.getIndex());
        });
        head = newHead.orElseGet(() -> targets.isEmpty() ? null : targets.getLast());
    }

    List<Cell> getTargets() { return targets; }

    void clearTargets(List<Cell> availableTargets) {
        if (head != null) {
            System.err.println(name + ": head cleared");
            head = null;
        }
        if (!targets.isEmpty()) {
            targets.stream().filter(Cell::hasResources).forEach(availableTargets::add);
            targets.clear();
            System.err.println(name + ": targets cleared");
        }
    }

    List<Link> constructPath(List<Cell> availableTargets) {
        path.clear();
        targets.forEach(c -> path.add(new Link(path.isEmpty() ? base : path.getLast().getTo(), c)));
        System.err.println(name + ": links before search");
        if (!path.isEmpty()) {
            System.err.printf("%d", path.getFirst().getFrom().getIndex());
            for (Link l : path)
                System.err.print(" ->- " + l.getTo().getIndex());
            System.err.println();
        }

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
        if (!path.isEmpty()) {
            System.err.printf("%d", path.getFirst().getFrom().getIndex());
            for (Link l : path)
                System.err.print(" ->- " + l.getTo().getIndex());
            System.err.println();
        }

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

    Cell getFrom() { return from; }

    Cell getTo() { return to; }

    int getLength() { return length; }

}

enum Strategy {

    EGGHOARDER(1) {
        Optional<Cell> selectNextTarget(Cell head, List<Cell> availableTargets) {
            List<Cell> availableEggs = availableTargets.stream()
                    .filter(c -> c.getResource().isType(Resource.EGGS))
                    .collect(Collectors.toList());
            if (availableEggs.isEmpty()) return Optional.empty();
            return Optional.ofNullable(head)
                    .map(c -> AntBrain.findClosestTo(c, availableEggs))
                    .orElse(availableEggs.stream().findFirst());
        } },
    GATHERER (2) {
        Optional<Cell> selectNextTarget(Cell head, List<Cell> availableTargets) {
            if (availableTargets.isEmpty()) return Optional.empty();
            return Optional.ofNullable(head)
                    .map(c -> AntBrain.findClosestTo(c, availableTargets))
                    .orElse(availableTargets.stream().findFirst());
        } },
    CRYSTALOBSESSION(1) {
        Optional<Cell> selectNextTarget(Cell head, List<Cell> availableTargets) {
            List<Cell> availableCrystals = availableTargets.stream()
                    .filter(c -> c.getResource().isType(Resource.CRYSTAL))
                    .collect(Collectors.toList());
            if (availableCrystals.isEmpty()) return Optional.empty();
            return Optional.ofNullable(head)
                    .map(c -> AntBrain.findClosestTo(c, availableCrystals))
                    .orElse(availableCrystals.stream().findFirst());
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

    boolean richDeposit() {
        return quantity >= myAnts;
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

    private final int type;

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