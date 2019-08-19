package uk.ac.bris.cs.scotlandyard.ui.ai;

import static java.lang.Double.NEGATIVE_INFINITY;
import static java.lang.Double.POSITIVE_INFINITY;
import java.util.ArrayList;
import java.util.Collection;
import java.util.HashMap;
import java.util.List;
import java.util.Optional;
import java.util.Set;
import java.util.concurrent.CopyOnWriteArrayList;
import java.util.function.Consumer;
import uk.ac.bris.cs.gamekit.graph.Edge;
import uk.ac.bris.cs.gamekit.graph.Graph;
import uk.ac.bris.cs.gamekit.graph.Node;

import uk.ac.bris.cs.scotlandyard.ai.ManagedAI;
import uk.ac.bris.cs.scotlandyard.ai.PlayerFactory;
import uk.ac.bris.cs.scotlandyard.model.Colour;
import uk.ac.bris.cs.scotlandyard.model.Move;
import uk.ac.bris.cs.scotlandyard.model.Player;
import uk.ac.bris.cs.scotlandyard.model.ScotlandYardView;
import uk.ac.bris.cs.scotlandyard.model.Ticket;
import static uk.ac.bris.cs.scotlandyard.model.Ticket.BUS;
import static uk.ac.bris.cs.scotlandyard.model.Ticket.DOUBLE;
import static uk.ac.bris.cs.scotlandyard.model.Ticket.SECRET;
import static uk.ac.bris.cs.scotlandyard.model.Ticket.TAXI;
import static uk.ac.bris.cs.scotlandyard.model.Ticket.UNDERGROUND;
import uk.ac.bris.cs.scotlandyard.model.Transport;
import static uk.ac.bris.cs.scotlandyard.model.Transport.FERRY;

// TODO name the AI
@ManagedAI("Mr X AI")
public class MrXAI implements PlayerFactory {
    
	// TODO create a new player here
	@Override
	public Player createPlayer(Colour colour) {
                MyPlayer player = new MyPlayer();                
		return player;
	}
        

	private static class MyPlayer implements Player {

                private HashMap<Node<Integer>, HashMap<Node<Integer>, Integer>> distances = new HashMap<>();
                private Graph<Integer, Transport> graph;
                private List<Boolean> rounds;
                private int MrXLocation;
                private Move bestMove;
                private int depth;
                private int firstDepth;
                

		@Override
		public void makeMove(ScotlandYardView view, int location, Set<Move> moves,
				Consumer<Move> callback) {
			//if this is the first move of the game, populate the Map containing distances from all nodes to all other nodes
                        MrXLocation = location;
                        if(view.getCurrentRound()==0) initialize(view);
                        minimax(getAIBoard(view), depth, NEGATIVE_INFINITY, POSITIVE_INFINITY);
			callback.accept(bestMove);
		}
                
                //stores a reference to the game graph for a given view and calls getAllDistances for it, to find the distance from all of its nodes to each of its other nodes
                private void initialize(ScotlandYardView view) {
                        graph = view.getGraph();
                        rounds = view.getRounds();
                        getAllDistances(graph);
                        depth = view.getPlayers().size()+1; //sets minimax tree depth so that it will evaluate after to just after Mr. X's move after next
                        firstDepth = depth;
                }
                
                /**
                 * Uses provided Scotland yard view, to get all the details needed to create an AIBoard object,
                 * fetches all the players ticket values and adds the to a list of AIPlayers. Then uses this list
                 * and other data from the ScotlandYardView to create and return a new AIBoard
                 * 
                 * @param view The ScotlandYardView to use as a basis for the AIBoard
                 * @return The AIBoard created with the provided ScotlandYardView
                 */
                private AIBoard getAIBoard(ScotlandYardView view) {
                        ArrayList<AIPlayer> AIPlayers = new ArrayList<>();
                        //Loops through all current players, gets how many of each ticket they have, and stores this info in the AIPlayers list
                        for (Colour player : view.getPlayers()) {
                            HashMap<Ticket, Integer> tickets = new HashMap<>();
                            //can safely assume the ticket optionals are present as .getPlayerTickets is defined to only return empty when there is no player of the specified colour
                            tickets.put(TAXI, view.getPlayerTickets(player, TAXI).get());
                            tickets.put(BUS, view.getPlayerTickets(player, BUS).get());
                            tickets.put(UNDERGROUND, view.getPlayerTickets(player, UNDERGROUND).get());
                            tickets.put(DOUBLE, view.getPlayerTickets(player, DOUBLE).get());
                            tickets.put(SECRET, view.getPlayerTickets(player, SECRET).get());
                            if (player.isMrX()) AIPlayers.add(new AIPlayer(player, MrXLocation, tickets));
                            else AIPlayers.add(new AIPlayer(player, view.getPlayerLocation(player).get(), tickets));
                        }
                        return new AIBoard(AIPlayers, view.getCurrentRound(), view.getRounds(), view.getCurrentPlayer(), graph);
                }
                
                /**
                 * Minimax algorithm to determine the best move for the AI to make
                 * sets global bestMove variable to store the currently determined bestMove
                 * 
                 * @param board The board the game is currently being played on
                 * @param depth The maximum number of recursive calls that can be made
                 * @param alpha The best possible value for the maximisers to take to the root, defaults to negative infinity
                 * @param beta The best possible value for the minimisers to take to the root, defaults to positive infinity
                 * @return Score to be used in recursive calls to determine best move
                 */
                private double minimax(AIBoard board, int depth, double alpha, double beta) {
                        //check if Mr. X has won or lost in this simulation and adjust score appropriately
                        if (board.checkDetectivesLose()) return POSITIVE_INFINITY;
                        if (board.checkMrXLose()) return NEGATIVE_INFINITY;
                        if (depth == 0) return scoreBoard(board);
                        Set<Move> validMoves = board.getValidMoves();
                        double tempScore;
                        
                        if (board.getCurrentPlayer().isMrX()) { //Assumes MrX makes optimal move                        
                            //maximizing, so anything above -infinity is an improvement
                            tempScore = NEGATIVE_INFINITY;
                            for (Move m : validMoves) {
                                AIBoard tempBoard = movePlayer(board, m);
                                //advance the rounds since Mr. X just went
                                tempBoard.setCurrentRound(tempBoard.getCurrentRound() + 1);
                                //recursion
                                tempScore = Double.max(minimax(tempBoard, depth - 1, alpha, beta), tempScore);
                                if (tempScore >= alpha) {
                                    alpha = tempScore;                                    
                                    //set bestMove if it is the first called iteration of minimax and a new best score has been found
                                    if (depth == firstDepth) bestMove = m;
                                    //alpha beta pruning 
                                    if (alpha >= beta) break;
                                }
                                                               
                            }
                            return alpha;   
                        }
                        else { //Assumes detective makes optimal move
                            //Minimizing, finding lowest score available
                            tempScore = POSITIVE_INFINITY;
                            for (Move m : validMoves) {
                                //System.out.println("\nConsidering move: " + m);
                                AIBoard tempBoard = movePlayer(board, m);
                                tempScore = Double.min(minimax(tempBoard, depth - 1, alpha, beta), tempScore) ;
                                //If the score of the searched tree is smaller than the current best route for the minimiser, beta is updated
                                if (tempScore <= beta) {
                                    beta = tempScore;
                                    //If the best maximiser route in the entire game tree is better than the current 
                                    if (alpha >= beta) break;
                                }                                
                            }
                            return beta; 
                        }                            
                }
                
                /**
                 * moves an AIPlayer and adjusts their ticket count then creates and returns a copy of the AIBoard it was passed 
                 * with the updated player location and currentPlayer. No movement if called with a PassMove
                 * 
                 * @param board The AIBoard that is to be updated with the provided move
                 * @param m The move provided for the currentPlayer to make
                 * @return The updated AIBoard after the player has made their move
                 */
                private AIBoard movePlayer(AIBoard board, Move m) {
                    
                        //get current player from AIBoard
                        AIPlayer player;
                        if (board.getCurrentPlayerAsAIPlayer().isPresent()) {
                            player = board.getCurrentPlayerAsAIPlayer().get();
                        }
                        else throw new IllegalArgumentException("Board has no current player");
                        
                        //update player location and ticket count
                        AIMoveVisitor visitor = new AIMoveVisitor();
                        Optional<Integer> destination = getNewLocation(m, visitor);
                        if (destination.isPresent()) player.location(destination.get());
                        if (visitor.getT1().isPresent()) player.removeTicket(visitor.getT1().get());
                        if (visitor.getT2().isPresent()) player.removeTicket(visitor.getT2().get());
                        
                        //update player list with newly updated currentPlayer
                        List<AIPlayer> players = board.getPlayers();
                        int playerIndex = players.indexOf(player);
                        players.set(playerIndex, player);
                        
                        //create and return a new AIBoard with the updated player, with the currentPlayer as the next player
                        return new AIBoard(players, board.getCurrentRound(), rounds, m, players.get((playerIndex + 1) % players.size()).colour(), graph);
                }                                       
                
                /**
                 * finds the final destination of a move and returns it using AIMoveVisitor; uses an AIMoveVisitor passed to it instead of creating a new one
                 * 
                 * @param move The move to find the final destination of
                 * @param visitor The AIMoveVisitor we want to use to visit the move
                 * @return The final destination of the provided move
                 */
                private Optional<Integer> getNewLocation(Move move, AIMoveVisitor visitor) {
                        move.visit(visitor);
                        return visitor.getDestination();
                }
                
                /**
                 * Evaluates the supplied board positions and returns a score indicating how good a position Mr. X is in
                 * 
                 * @param board The game board used to evaluate MrXs score based on player positions
                 * @return The calculated score
                 */
                private double scoreBoard (AIBoard board) {
                        List<Integer> distanceToMrX = new ArrayList<>();
                        double score = 0;
                        Transport transport;
                                                
                        //iterate through list of all players and find the distance between their current location and Mr. X
                        for (int playerLocation : board.getPlayerLocations()) {
                                distanceToMrX.add(distances.get(graph.getNode(playerLocation)).get(graph.getNode(board.getMrXLocation())));
                        }
                        
                        //applies value of weighted distances to the move's score
                        for (int distance : distanceToMrX) {
                            if (distance == 0) score = NEGATIVE_INFINITY;
                            //heuristic for moves beyond scope of minimax tree; if Mr. X ends his move one turn away from a detective, that is BAD
                            if (distance == 1 && board.getCurrentPlayer().isDetective()) score -= 100000;
                            else score += (1000 * Math.log(distance));
                        }
                        
                        //increases score based upon number of different forms of transport available from Mr. X's location
                        for (Edge<Integer, Transport> edge : graph.getEdgesFrom(graph.getNode(board.getMrXLocation()))) {
                            transport = edge.data();
                            switch (transport) {
                                case BUS:
                                    score+=35;
                                    break;
                                    
                                case UNDERGROUND:
                                    score+=75;
                                    break;
                                    
                                case FERRY:
                                    score+=155;
                                    break;     
                                
                                default:
                                    break;
                            }                                    
                        }
                        
                        //increases score based on rarity of ticket used; small amount of extra value given to moves that use taxi tickets, are they are most abundant
                        //then bus, as they're next most abundant, etc.
                        if (board.getMove().isPresent()) {
                            AIMoveVisitor ticketVisitor  = new AIMoveVisitor();
                            board.getMove().get().visit(ticketVisitor);
                            if (ticketVisitor.getT1().isPresent()) {
                                switch (ticketVisitor.getT1().get()) {
                                    case TAXI:
                                        score +=15;
                                        break;
                                    case BUS:
                                        score +=10;
                                        break;
                                    case UNDERGROUND:
                                        score+=5;
                                        break;
                                }
                            }
                            
                            if (ticketVisitor.getT2().isPresent()) {
                                //adjusts score preemptively so double moves are not always prioritized
                                score-= 15;
                                switch (ticketVisitor.getT2().get()) {
                                    case TAXI:
                                        score +=15;
                                        break;
                                    case BUS:
                                        score +=10;
                                        break;
                                    case UNDERGROUND:
                                        score+=5;
                                        break;
                                }
                            }
                        }
                        
                        return score;
                }
                
                /**
                 * finds the distance from every node in the specificed graph to every other node
                 * 
                 * @param graph The graph we want distances for
                 */
                private void getAllDistances(Graph<Integer, Transport> graph) {
                        for (Node<Integer> node : graph.getNodes()) {
                            distances.put(node, getDistances(node, graph));
                        }
                }
                
                /**
                 * returns the distance from a specified node to all other nodes on a given graph as a HashMap. Uses a breadth first search
                 * 
                 * @param n1 Node to find distances to
                 * @param graph Graph containing node we want to find distances to
                 * @return Hash map containing the distances to the given node for every node on the graph
                 */
                private HashMap<Node<Integer>, Integer> getDistances(Node<Integer> n1, Graph<Integer, Transport> graph) {
                        //HashMap containing distances of all nodes from loc1, using node as key and distance as value
                        HashMap <Node<Integer>, Integer> distanceMap = new HashMap<>();
                        List<Node<Integer>> visitedNodes = new ArrayList<>();
                        List<Node<Integer>> nodesToVisit = new CopyOnWriteArrayList<>();
                        //adds loc1 to hashmap with distance of 0
                        distanceMap.put(n1, 0);
                        visitedNodes.add(n1);
                        nodesToVisit.add(n1);
                        Collection<Edge<Integer, Transport>> tempEdges;
                                                
                        
                        //iterates through each adjacent node to loc1 and then recurses through the adjacent nodes to each of those to get the distance from loc1 to all nodes
                        while (! nodesToVisit.isEmpty()) {
                            Node<Integer> node = nodesToVisit.remove(0);
                            tempEdges = graph.getEdgesFrom(node);
                            for (Edge<Integer, Transport> edge : tempEdges) {
                                //ignores ferry links as detectives can't use them
                                if (! visitedNodes.contains(edge.destination()) && edge.data() != FERRY) {
                                    //breadth first search, so each node will be 1 further away from loc1 than its 'parent' node
                                    distanceMap.put(edge.destination(), (distanceMap.get(node) + 1));
                                    nodesToVisit.add(edge.destination());
                                    visitedNodes.add(edge.destination());   
                                }
                            }
                                                     
                        }

                        return distanceMap;
                }
                

                
                
                
	}
}

