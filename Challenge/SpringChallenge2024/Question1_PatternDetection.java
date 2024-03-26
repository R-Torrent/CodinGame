package Challenge.SpringChallenge2024;

import java.util.*;
import java.util.stream.Collectors;

import com.google.gson.Gson;
import com.google.gson.GsonBuilder;
import com.google.gson.reflect.TypeToken;

public class Question1_PatternDetection {
    /**
     * @param n The number of buildings
     * @param buildingMap Representation of the n buildings
     * @return The height of each building.
     */
    public static List<Integer> buildingHeights(int n, List<String> buildingMap) {
        // Write your code here
        return buildingMap.stream().map(String::trim).mapToInt(String::length).boxed().collect(Collectors.toList());
    }

    /* Ignore and do not change the code below */
    private static final Gson gson = new GsonBuilder().disableHtmlEscaping().create();

    /**
     * Try a solution
     * @param output The height of each building.
     */
    public static void trySolution(List<Integer> output) {
        System.out.println("" + gson.toJson(output));
    }

    public static void main(String args[]) {
        try (Scanner in = new Scanner(System.in)) {
            trySolution(buildingHeights(
                    gson.fromJson(in.nextLine(), new TypeToken<Integer>(){}.getType()),
                    gson.fromJson(in.nextLine(), new TypeToken<List<String>>(){}.getType())
            ));
        }
    }
    /* Ignore and do not change the code above */

}
