package Challenge.SummerChallenge2023.EnjoyingTheLastDaysInSchool;

import java.util.*;
import com.google.gson.Gson;
import com.google.gson.GsonBuilder;
import com.google.gson.reflect.TypeToken;
import com.google.gson.annotations.SerializedName;

public class Question10 {

    /**
     * @param nMutants The number of graduating mutants
     * @param wishlistY The priority of mutants selected by Y-Men
     * @param wishlistZ The priority of mutants selected by Z-Men
     * @param wishlistMutants The priorities of squads for each mutant
     * @param friendships The list of friendships between mutants
     * @return The best possible score across all configurations
     */
    public static int maxScore(int nMutants, Map<Integer, Double> wishlistY, Map<Integer, Double> wishlistZ, List<Unknown> wishlistMutants, List<Unknown2> friendships) {
        // Write your code here

        return 160;
    }

    /* Ignore and do not change the code below */
    private static final Gson gson = new GsonBuilder().disableHtmlEscaping().create();

    /**
     * Try a solution
     * @param maxPossibleScore The best possible score across all configurations
     */
    public static void trySolution(int maxPossibleScore) {
        System.out.println("" + gson.toJson(maxPossibleScore));
    }

    public static void main(String args[]) {
        try (Scanner in = new Scanner(System.in)) {
            trySolution(maxScore(
                    gson.fromJson(in.nextLine(), new TypeToken<Integer>(){}.getType()),
                    gson.fromJson(in.nextLine(), new TypeToken<Map<Integer, Double>>(){}.getType()),
                    gson.fromJson(in.nextLine(), new TypeToken<Map<Integer, Double>>(){}.getType()),
                    gson.fromJson(in.nextLine(), new TypeToken<List<Unknown>>(){}.getType()),
                    gson.fromJson(in.nextLine(), new TypeToken<List<Unknown2>>(){}.getType())
            ));
        }
    }
    /* Ignore and do not change the code above */

}

class Unknown {

    @SerializedName("mutant")
    public String mutant;
    @SerializedName("squad")
    public String squad;
    @SerializedName("priority")
    public int priority;

}

class Unknown2 {

    @SerializedName("friend1")
    public String friend1;
    @SerializedName("friend2")
    public String friend2;
    @SerializedName("strength")
    public int strength;

}