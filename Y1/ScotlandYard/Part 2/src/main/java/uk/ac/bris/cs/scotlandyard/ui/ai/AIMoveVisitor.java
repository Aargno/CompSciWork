/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package uk.ac.bris.cs.scotlandyard.ui.ai;

import java.util.Optional;
import uk.ac.bris.cs.scotlandyard.model.DoubleMove;
import uk.ac.bris.cs.scotlandyard.model.MoveVisitor;
import uk.ac.bris.cs.scotlandyard.model.PassMove;
import uk.ac.bris.cs.scotlandyard.model.Ticket;
import uk.ac.bris.cs.scotlandyard.model.TicketMove;

/**
 * Move visitor to store the details of the move being visited
 * 
 * @author Limited
 */
public class AIMoveVisitor implements MoveVisitor{
       
    private Optional<Integer> destination = Optional.empty(); //will remain empty after visit if a PassMove is visited
    private Optional<Ticket> t1 = Optional.empty(); //will remain empty after visit if a PassMove is visited
    private Optional<Ticket> t2 = Optional.empty(); //will remain empty after visit if a PassMove or TicketMove is visited
    
    /**
     * Visitor for PassMove, retrieves all relevant data from the move
     * 
     * @param move The PassMove being visited
     */
    @Override
    public void visit(PassMove move) {
        //No details for pass move, defauls values kept
    }
    
    /**
     * Visitor for TicketMove, retrieves all relevant data from the move
     * 
     * @param move The TicketMove being visited
     */
    @Override
    public void visit(TicketMove move) {
        destination = Optional.of(move.destination());
        t1 = Optional.of(move.ticket());
    }
    
    /**
     * Visitor for DoubleMove, retrieves the final destination of the move,
     * and stores the two individual moves that make up the move
     * 
     * @param move The DoubleMove being visited
     */
    @Override
    public void visit(DoubleMove move) {
        destination = Optional.of(move.finalDestination());
        t1 = Optional.of(move.firstMove().ticket());
        t2 = Optional.of(move.secondMove().ticket());
    }
    
    /**
     * Retrieves the destination of the move last visited by this visitor
     * 
     * @return The integer representing the destination of the last move visited
     */
    public Optional<Integer> getDestination() {
        return destination;
    }

    /**
     * Returns the first ticket used in the last visited double move
     * 
     * @return First ticket used in last visited double move
     */
    public Optional<Ticket> getT1() {
        return t1;
    }

    /**
     * Returns the second ticket used in the last visited double move
     * 
     * @return Second ticket used in last visited double move
     */
    public Optional<Ticket> getT2() {
        return t2;
    }
    
}
